// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ModelManager.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <system_error>
#include <windows.h>

namespace
{
    std::filesystem::path CreateTemporaryCompiledModelPath(const std::filesystem::path& compiledModelPath)
    {
        const auto uniqueValue = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        const std::wstring temporaryName =
            compiledModelPath.stem().wstring() + L".tmp." + std::to_wstring(GetCurrentProcessId()) + L"." +
            std::to_wstring(uniqueValue) + compiledModelPath.extension().wstring();
        return compiledModelPath.parent_path() / temporaryName;
    }
}

namespace WindowsML
{
namespace Shared
{

    OrtStatus* ModelManager::CompileModel(
        const OrtApi& ortApi,
        Ort::Env& env,
        OrtSessionOptions* sessionOptions,
        const std::filesystem::path& modelPath,
        const std::filesystem::path& compiledModelPath)
    {
        std::cout << "Compiling model from " << modelPath << std::endl;
        std::cout << "Output path: " << compiledModelPath << std::endl;

        std::error_code pathError;
        const bool compiledModelExists = std::filesystem::exists(compiledModelPath, pathError);
        if (pathError)
        {
            const std::string message =
                "Failed to inspect compiled model output path: " + pathError.message();
            std::cerr << message << std::endl;
            return ortApi.CreateStatus(ORT_FAIL, message.c_str());
        }

        if (compiledModelExists && std::filesystem::equivalent(modelPath, compiledModelPath, pathError))
        {
            const std::string message = "Compilation output path must be different from the original model path.";
            std::cerr << message << std::endl;
            return ortApi.CreateStatus(ORT_INVALID_ARGUMENT, message.c_str());
        }
        if (pathError)
        {
            const std::string message =
                "Failed to compare input and compiled model paths: " + pathError.message();
            std::cerr << message << std::endl;
            return ortApi.CreateStatus(ORT_FAIL, message.c_str());
        }

        // Get compile API
        const OrtCompileApi* compileApi = ortApi.GetCompileApi();
        if (!compileApi)
        {
            std::cerr << "Failed to get compile API" << std::endl;
            return ortApi.CreateStatus(ORT_FAIL, "Failed to get compile API");
        }

        const std::filesystem::path temporaryModelPath = CreateTemporaryCompiledModelPath(compiledModelPath);
        std::error_code cleanupError;
        std::filesystem::remove(temporaryModelPath, cleanupError);

        // Create compilation options from session options
        OrtModelCompilationOptions* compileOptions = nullptr;
        OrtStatus* status = compileApi->CreateModelCompilationOptionsFromSessionOptions(env, sessionOptions, &compileOptions);
        if (status != nullptr)
        {
            std::cerr << "Failed to create compilation options: " << ortApi.GetErrorMessage(status) << std::endl;
            return status;
        }

        // Set input and output model paths
        status = compileApi->ModelCompilationOptions_SetInputModelPath(compileOptions, modelPath.c_str());
        if (status != nullptr)
        {
            std::cerr << "Failed to set input model path: " << ortApi.GetErrorMessage(status) << std::endl;
            compileApi->ReleaseModelCompilationOptions(compileOptions);
            return status;
        }

        status = compileApi->ModelCompilationOptions_SetOutputModelPath(compileOptions, temporaryModelPath.c_str());
        if (status != nullptr)
        {
            std::cerr << "Failed to set output model path: " << ortApi.GetErrorMessage(status) << std::endl;
            compileApi->ReleaseModelCompilationOptions(compileOptions);
            return status;
        }

        // Keep the compiled EP context in the ONNX file so atomic replacement
        // cannot strand a model that references a temp-named sidecar.
        status = compileApi->ModelCompilationOptions_SetEpContextEmbedMode(compileOptions, true);
        if (status != nullptr)
        {
            std::cerr << "Failed to configure embedded EP context: " << ortApi.GetErrorMessage(status) << std::endl;
            compileApi->ReleaseModelCompilationOptions(compileOptions);
            return status;
        }

        // Measure compile time
        std::cout << "Starting compile, this may take a few moments..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        // Compile the model
        status = compileApi->CompileModel(env, compileOptions);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        if (status == nullptr)
        {
            std::error_code outputError;
            const bool outputExists = std::filesystem::exists(temporaryModelPath, outputError);
            if (outputError)
            {
                const std::string message =
                    "Failed to inspect temporary compiled model: " + outputError.message();
                status = ortApi.CreateStatus(ORT_FAIL, message.c_str());
                std::cerr << message << std::endl;
            }
            else if (!outputExists)
            {
                status = ortApi.CreateStatus(ORT_FAIL, "Compilation completed without creating an output model");
                std::cerr << ortApi.GetErrorMessage(status) << std::endl;
            }
            else if (!MoveFileExW(
                         temporaryModelPath.c_str(),
                         compiledModelPath.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            {
                const DWORD error = GetLastError();
                const std::string message =
                    "Failed to replace compiled model atomically. Win32 error: " + std::to_string(error);
                status = ortApi.CreateStatus(ORT_FAIL, message.c_str());
                std::wcerr << L"Failed to replace temporary compiled model '" << temporaryModelPath
                           << L"' with cache '" << compiledModelPath << L"'. Win32 error: " << error << std::endl;
            }
            else
            {
                std::cout << "Model compiled successfully in " << duration.count() << " ms" << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to compile model: " << ortApi.GetErrorMessage(status) << std::endl;
        }

        compileApi->ReleaseModelCompilationOptions(compileOptions);
        if (status != nullptr)
        {
            std::filesystem::remove(temporaryModelPath, cleanupError);
        }
        return status;
    }

    std::filesystem::path ModelManager::GetModulePath(HMODULE module)
    {
        std::wstring buffer;

        for (std::uint32_t size{70}; size < 4096; size *= 2)
        {
            buffer.resize(size, L'\0');

            std::uint32_t requiredSize = ::GetModuleFileNameW(module, buffer.data(), size);
            if (requiredSize == 0)
            {
                return {};
            }

            if (requiredSize == size)
            {
                continue;
            }

            buffer.resize(requiredSize);
            return {std::move(buffer)};
        }

        return {};
    }

    std::filesystem::path ModelManager::GetExecutablePath()
    {
        return GetModulePath(nullptr);
    }

    std::vector<std::string> ModelManager::LoadLabels(const std::filesystem::path& labelsPath)
    {
        std::ifstream labelFile{labelsPath, std::ifstream::in};
        if (labelFile.fail())
        {
            throw std::runtime_error("Unable to load labels file.");
        }

        std::vector<std::string> labels(1000);
        for (std::string s; std::getline(labelFile, s, ',');)
        {
            int labelValue = stoi(s);
            if (labelValue >= labels.size())
            {
                labels.resize(labelValue + 1);
            }
            std::getline(labelFile, s);
            labels[labelValue] = s;
        }

        return labels;
    }

    void ModelManager::SetDefaultPaths(
        const std::filesystem::path& executableFolder, const std::wstring& modelPath, std::wstring& outputModelPath, std::wstring& outputImagePath)
    {
        // Set default compiled model path if not specified
        if (outputModelPath.empty())
        {
            auto pos = modelPath.rfind(L".");
            if (pos != std::wstring::npos)
            {
                outputModelPath = std::wstring(modelPath.substr(0, pos)) + L"_ctx.onnx";
            }
            else
            {
                outputModelPath = modelPath + L"_ctx.onnx";
            }
        }

        // Set default image path if not specified
        if (outputImagePath.empty())
        {
            outputImagePath = executableFolder / L"image.png";
        }
    }

    std::wstring ModelManager::GetModelVariantPath(
        const std::filesystem::path& executableFolder,
        ModelVariant variant)
    {
        std::wstring modelPath;
        
        switch (variant)
        {
            case ModelVariant::FP32:
                modelPath = executableFolder / L"SqueezeNet.fp32.onnx";
                break;
            case ModelVariant::Default:
            default:
                modelPath = executableFolder / L"SqueezeNet.onnx";
                break;
        }

        std::wcout << L"Using model variant: " << (variant == ModelVariant::FP32 ? L"FP32" : L"Default") << L" -> " << modelPath << std::endl;
        return modelPath;
    }

    ModelVariant ModelManager::DetermineModelVariant(const CommandLineOptions& options, Ort::Env& env)
    {
        // If user explicitly set a custom model path, variant doesn't matter
        if (!options.model_path.empty())
        {
            return options.model_variant; // Use whatever was set (default is Default)
        }

        // For EP policy, we can determine based on the policy
        if (options.ep_policy.has_value())
        {
            ModelVariant variant;
            switch (options.ep_policy.value())
            {
                case OrtExecutionProviderDevicePolicy_PREFER_GPU:
                    variant = ModelVariant::FP32;
                    std::wcout << L"Auto-selected FP32 model variant for GPU execution\n";
                    break;
                case OrtExecutionProviderDevicePolicy_PREFER_NPU:
                case OrtExecutionProviderDevicePolicy_PREFER_CPU:
                case OrtExecutionProviderDevicePolicy_DEFAULT:
                default:
                    variant = ModelVariant::Default;
                    std::wcout << L"Auto-selected Default model variant for NPU/CPU execution\n";
                    break;
            }
            return variant;
        }

        // For explicit EP name, we need to check what device types are available
        if (!options.ep_name.empty())
        {
            try
            {
                std::vector<Ort::ConstEpDevice> devices = env.GetEpDevices();
                
                // If user specified a device type, use that
                if (options.device_type.has_value())
                {
                    ModelVariant variant = (options.device_type.value() == L"GPU") ? ModelVariant::FP32 : ModelVariant::Default;
                    std::wcout << L"Auto-selected " << (variant == ModelVariant::FP32 ? L"FP32" : L"Default") 
                              << L" model variant for " << options.ep_name << L" with device type " << options.device_type.value() << std::endl;
                    return variant;
                }
                
                // Otherwise, check if any of the available devices for this EP are GPU
                for (const auto& device : devices)
                {
                    // Convert wide string to narrow string properly
                    std::string epName;
                    size_t convertedChars = 0;
                    char buffer[256];
                    if (wcstombs_s(&convertedChars, buffer, sizeof(buffer), options.ep_name.c_str(), _TRUNCATE) == 0)
                    {
                        epName = buffer;
                    }
                    
                    if (device.EpName() == epName && device.Device().Type() == OrtHardwareDeviceType_GPU)
                    {
                        std::wcout << L"Auto-selected FP32 model variant for " << options.ep_name << L" (GPU device available)\n";
                        return ModelVariant::FP32;
                    }
                }
            }
            catch (const std::exception& ex)
            {
                std::wcout << L"Warning: Could not determine device type for " << options.ep_name << L": " 
                          << std::wstring(ex.what(), ex.what() + strlen(ex.what())) << std::endl;
            }
        }

        // Default fallback
        std::wcout << L"Auto-selected Default model variant (fallback)\n";
        return ModelVariant::Default;
    }

    std::filesystem::path ModelManager::GenerateCompiledModelPath(
        const std::filesystem::path& modelPath,
        const std::filesystem::path& executableFolder,
        const CommandLineOptions& options)
    {
        // If user explicitly specified --compiled_output, use it as-is
        if (!options.output_path.empty())
        {
            return std::filesystem::path(options.output_path);
        }

        std::wstring baseName = modelPath.stem().wstring();
        std::wstring suffix;

        if (options.ep_policy.has_value())
        {
            std::string policyStr = ArgumentParser::ToString(options.ep_policy.value());
            suffix = L"_" + std::wstring(policyStr.begin(), policyStr.end());
        }
        else if (!options.ep_name.empty())
        {
            suffix = L"_" + options.ep_name;

            // Try to determine device type
            std::wstring deviceType;
            if (options.device_type.has_value())
            {
                deviceType = options.device_type.value();
            }

            if (!deviceType.empty())
            {
                suffix += L"_" + deviceType;
            }
        }

        if (options.perf_mode == PerformanceMode::MaxPerformance)
        {
            suffix += L"_MaxPerformance";
        }
        else if (options.perf_mode == PerformanceMode::MaxEfficiency)
        {
            suffix += L"_MaxEfficiency";
        }

        std::wstring fileName = baseName + L"_ctx" + suffix + L".onnx";
        auto result = executableFolder / fileName;
        std::wcout << L"Compiled model path: " << result.wstring() << std::endl;
        return result;
    }

} // namespace Shared
} // namespace WindowsML
