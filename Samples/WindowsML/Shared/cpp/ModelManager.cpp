// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ModelManager.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <windows.h>

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

        // Get compile API
        const OrtCompileApi* compileApi = ortApi.GetCompileApi();
        if (!compileApi)
        {
            std::cerr << "Failed to get compile API" << std::endl;
            return nullptr;
        }

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

        status = compileApi->ModelCompilationOptions_SetOutputModelPath(compileOptions, compiledModelPath.c_str());
        if (status != nullptr)
        {
            std::cerr << "Failed to set output model path: " << ortApi.GetErrorMessage(status) << std::endl;
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
            std::cout << "Model compiled successfully in " << duration.count() << " ms" << std::endl;
        }
        else
        {
            std::cerr << "Failed to compile model: " << ortApi.GetErrorMessage(status) << std::endl;
        }

        compileApi->ReleaseModelCompilationOptions(compileOptions);
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

} // namespace Shared
} // namespace WindowsML
