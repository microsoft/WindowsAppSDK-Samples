// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "InferenceEngine.h"
#include "ExecutionProviderManager.h"
#include "ModelManager.h"
#include <iostream>

namespace WindowsML
{
namespace Shared
{

    Ort::SessionOptions InferenceEngine::CreateSessionOptions(const CommandLineOptions& options, Ort::Env& env)
    {
        Ort::SessionOptions sessionOptions;

        if (options.ep_policy.has_value())
        {
            // Set the EP selection policy based on command line
            std::cout << "Using EP Selection Policy: " << ArgumentParser::ToString(options.ep_policy.value()) << std::endl;
            sessionOptions.SetEpSelectionPolicy(options.ep_policy.value());
        }
        else
        {
            // Use explicit configuration
            std::cout << "Using explicit EP configuration" << std::endl;
            ExecutionProviderManager::ConfigureSelectedExecutionProvider(sessionOptions, env, options.ep_name, options.device_type);
        }

        return sessionOptions;
    }

    std::filesystem::path InferenceEngine::DetermineModelPath(
        const CommandLineOptions& options,
        const std::filesystem::path& modelPath,
        const std::filesystem::path& compiledModelPath,
        Ort::SessionOptions& sessionOptions,
        Ort::Env& env)
    {
        std::filesystem::path actualModelPath;
        bool isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);

        if (isCompiledModelAvailable)
        {
            std::cout << "Using existing compiled model: " << compiledModelPath << std::endl;
            actualModelPath = compiledModelPath;
        }
        else if (options.compile_model)
        {
            std::cout << "No compiled model found, attempting to create compiled model" << std::endl;

            const OrtApi& ortApi = Ort::GetApi();
            OrtStatus* status = ModelManager::CompileModel(ortApi, env, sessionOptions, modelPath, compiledModelPath);

            if (status == nullptr && std::filesystem::exists(compiledModelPath))
            {
                std::cout << "Compiled model created successfully at " << compiledModelPath << std::endl;
                actualModelPath = compiledModelPath;
            }
            else
            {
                std::cout << "Falling back to original model: " << modelPath << std::endl;
                actualModelPath = modelPath;
                if (status != nullptr)
                {
                    ortApi.ReleaseStatus(status);
                }
            }
        }
        else
        {
            std::cout << "Using original model: " << modelPath << std::endl;
            actualModelPath = modelPath;
        }

        return actualModelPath;
    }

    Ort::Value InferenceEngine::CreateInputTensor(const std::vector<float>& inputTensorValues, const std::vector<int64_t>& inputShape)
    {
        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        return Ort::Value::CreateTensor<float>(
            memoryInfo, const_cast<float*>(inputTensorValues.data()), inputTensorValues.size(), inputShape.data(), inputShape.size());
    }

    std::vector<Ort::Value> InferenceEngine::RunInference(Ort::Session& session, const char* inputName, const char* outputName, Ort::Value& inputTensor)
    {
        const char* inputNames[] = {inputName};
        const char* outputNames[] = {outputName};

        std::cout << "Running inference ..." << std::endl;
        return session.Run(Ort::RunOptions{nullptr}, inputNames, &inputTensor, 1, outputNames, 1);
    }

    std::vector<float> InferenceEngine::ExtractResults(std::vector<Ort::Value>& outputTensors)
    {
        float* outputData = outputTensors[0].GetTensorMutableData<float>();
        const size_t outputCount = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
        return std::vector<float>(outputData, outputData + outputCount);
    }

    std::vector<int64_t> InferenceEngine::PrepareInputShape(const std::vector<int64_t>& originalShape)
    {
        std::vector<int64_t> inputShape = originalShape;

        // For dynamic dimensions, set them to fixed values
        for (size_t i = 0; i < inputShape.size(); i++)
        {
            if (inputShape[i] == -1)
            {
                inputShape[i] = 1; // Assume batch size of 1
            }
        }

        return inputShape;
    }

} // namespace Shared
} // namespace WindowsML
