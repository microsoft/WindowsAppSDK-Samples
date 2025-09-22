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

} // namespace Shared
} // namespace WindowsML
