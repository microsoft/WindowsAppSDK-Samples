// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include "WindowsMLShared.h"

using namespace winrt::Windows::Foundation;
using namespace WindowsML::Shared;

IAsyncAction RunInferenceAsync(const CommandLineOptions& options)
{
    try
    {
        // Initialize WinRT apartment and providers
        ExecutionProviderManager epManager;
        epManager.InitializeProviders(options.download_packages);

        // Create ONNX environment and session options
        auto env = Ort::Env();
        std::string versionStr = Ort::GetVersionString();
        std::wcout << L"ONNX Runtime Version: " << std::wstring(versionStr.begin(), versionStr.end()).c_str() << std::endl;

        auto sessionOptions = InferenceEngine::CreateSessionOptions(options, env);

        // Determine model paths - look in executable directory (files are copied during build)
        std::wstring executablePath = ModelManager::GetExecutablePath();
        std::filesystem::path executableFolder = std::filesystem::path(executablePath).parent_path();

        std::filesystem::path modelPath;
        if (options.model_path.empty())
        {
            // Use default SqueezeNet with variant selection
            modelPath = ModelManager::GetModelVariantPath(executableFolder, options.model_variant);
        }
        else
        {
            // Use user-specified model path
            modelPath = std::filesystem::path(options.model_path);
        }

        std::filesystem::path outputPath =
            options.output_path.empty() ? executableFolder / L"SqueezeNet_ctx.onnx" : std::filesystem::path(options.output_path);

        std::filesystem::path imagePath =
            options.image_path.empty() ? executableFolder / L"image.png" : std::filesystem::path(options.image_path);

        // Determine the actual model to use
        std::filesystem::path actualModelPath =
            InferenceEngine::DetermineModelPath(options, modelPath, outputPath, sessionOptions, env);

        // Create session
        std::wcout << L"Loading model: " << actualModelPath.wstring().c_str() << std::endl;
        Ort::Session session(env, actualModelPath.c_str(), sessionOptions);

        // Get model input details
        Ort::AllocatorWithDefaultOptions allocator;
        auto inputName = session.GetInputNameAllocated(0, allocator);
        auto outputName = session.GetOutputNameAllocated(0, allocator);

        auto inputTypeInfo = session.GetInputTypeInfo(0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> inputShape = InferenceEngine::PrepareInputShape(inputTensorInfo.GetShape());

        // Load and process image
        ImageProcessor imageProcessor;
        auto videoFrame = co_await imageProcessor.LoadImageFileAsync(winrt::hstring{imagePath.wstring()});
        std::vector<float> inputTensorValues = imageProcessor.BindVideoFrameAsTensor(videoFrame);

        // Create input tensor
        auto inputTensor = InferenceEngine::CreateInputTensor(inputTensorValues, inputShape);

        // Run inference
        std::wcout << L"Running inference..." << std::endl;
        auto outputTensors = InferenceEngine::RunInference(session, inputName.get(), outputName.get(), inputTensor);

        // Extract results
        std::vector<float> results = InferenceEngine::ExtractResults(outputTensors);

        // Load labels and display results
        std::filesystem::path labelsPath = executableFolder / "SqueezeNet.Labels.txt";
        std::vector<std::string> labels = ModelManager::LoadLabels(labelsPath);

        if (labels.empty())
        {
            std::wcout << L"Warning: Could not load labels. Using generic labels.\n";
            for (size_t i = 0; i < results.size() && i < 10; ++i)
            {
                labels.push_back("Class " + std::to_string(i));
            }
        }

        ResultProcessor::PrintResults(labels, results);
    }
    catch (std::exception const& ex)
    {
        std::wcout << L"Error: " << std::wstring(ex.what(), ex.what() + strlen(ex.what())).c_str() << std::endl;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    winrt::init_apartment();

    try
    {
        // Parse command line arguments
        CommandLineOptions options;
        if (!ArgumentParser::ParseCommandLineArgs(argc, argv, options))
        {
            return -1;
        }

        // Run the async inference
        RunInferenceAsync(options).get();

        std::wcout << L"\nPress any key to continue..." << std::endl;
        std::wcin.get();
    }
    catch (...)
    {
        std::wcout << L"An unexpected error occurred." << std::endl;
        return -1;
    }

    return 0;
}
