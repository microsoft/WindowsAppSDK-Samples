// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
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
        std::filesystem::path labelsPath;
        if (options.use_model_catalog)
        {
            // Build source
            std::wcout << L"Using model catalog..." << std::endl;

            auto sampleCatalogJsonPath = executableFolder / L"SqueezeNetModelCatalog.json";
            auto uri = winrt::Windows::Foundation::Uri(sampleCatalogJsonPath.c_str());
            auto sampleCatalogSource = winrt::Microsoft::Windows::AI::MachineLearning::CatalogModelSource::CreateFromUri(uri).get();

            winrt::Microsoft::Windows::AI::MachineLearning::WinMLModelCatalog modelCatalog({sampleCatalogSource});

            // Use intelligent model variant selection based on execution provider and device capabilities
            ModelVariant actualVariant = ModelManager::DetermineModelVariant(options, env);

            winrt::Microsoft::Windows::AI::MachineLearning::CatalogModelInfo modelFromCatalog{nullptr};

            std::wstring modelVariantName = (actualVariant == ModelVariant::FP32) ? L"squeezenet-fp32" : L"squeezenet";

            modelFromCatalog = modelCatalog.FindModel(modelVariantName.c_str()).get();

            if (modelFromCatalog != nullptr)
            {
                auto catalogModelInstanceOp = modelFromCatalog.GetInstance({});

                catalogModelInstanceOp.Progress([](auto const& /*operation*/, double progress) {
                    std::wcout << L"Model download progress: " << progress << L"%\r";
                });

                auto catalogModelInstanceResult = co_await catalogModelInstanceOp;

                if (catalogModelInstanceResult.Status() == winrt::Microsoft::Windows::AI::MachineLearning::CatalogModelStatus::Available)
                {
                    auto catalogModelInstance = catalogModelInstanceResult.Instance();
                    auto modelPaths = catalogModelInstance.ModelPaths();

                    auto modelFolderPath = std::filesystem::path(modelPaths.GetAt(0).c_str());
                    std::wstring modelName = modelVariantName + L".onnx";
                    modelPath = modelFolderPath / modelName;
                    std::wcout << L"Using model from catalog at: " << modelPath.c_str() << std::endl;

                    // Get labels
                    labelsPath = modelFolderPath / L"SqueezeNet.Labels.txt";
                }
                else
                {
                    std::wcout << L"Model download failed. Falling back to executableFolder" << std::endl;
                    modelPath = ModelManager::GetModelVariantPath(executableFolder, actualVariant);
                }
            }
            else
            {
                std::wcout << L"Model with alias or ID '" << modelVariantName.c_str() << L"' not found in catalog. Falling back to executableFolder" << std::endl;
                modelPath = ModelManager::GetModelVariantPath(executableFolder, actualVariant);
            }
        }
        else if (options.model_path.empty())
        {
            // Use intelligent model variant selection based on execution provider and device capabilities
            ModelVariant actualVariant = ModelManager::DetermineModelVariant(options, env);
            modelPath = ModelManager::GetModelVariantPath(executableFolder, actualVariant);
        }
        else
        {
            // Use user-specified model path
            modelPath = std::filesystem::path(options.model_path);
        }

        if (labelsPath.empty())
        {
            labelsPath = executableFolder / "SqueezeNet.Labels.txt";
        }

        // Load labels
        std::vector<std::string> labels = ModelManager::LoadLabels(labelsPath);

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
