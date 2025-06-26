// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ResnetModelHelper.hpp"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>
#include <win_onnxruntime_cxx_api.h>
#include <WinMLBootstrap.h>
#include <winrt/base.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

int wmain(int argc, wchar_t* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;

    winrt::init_apartment();
    // Initialize ONNX Runtime
    Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "CppConsoleDesktop");

    // Use WinML to download and register Execution Providers
    winrt::Microsoft::Windows::AI::MachineLearning::Infrastructure infrastructure;
    infrastructure.DownloadPackagesAsync().get();
    infrastructure.RegisterExecutionProviderLibrariesAsync().get();

    // Set the auto EP selection policy
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetEpSelectionPolicy(OrtExecutionProviderDevicePolicy_MIN_OVERALL_POWER);

    // Prepare paths for model and labels
    std::filesystem::path executableFolder = ResnetModelHelper::GetExecutablePath().parent_path();
    std::filesystem::path labelsPath = executableFolder / "ResNet50Labels.txt";
    std::filesystem::path dogImagePath = executableFolder / "dog.jpg";

    // TODO: use AITK Model Conversion tool to get resnet and paste the path here
    std::filesystem::path modelPath = L"";
    std::filesystem::path compiledModelPath = L"";
    bool isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);

    if (isCompiledModelAvailable)
    {
        std::cout << "Using compiled model: " << compiledModelPath << std::endl;
    }
    else
    {
        std::cout << "No compiled model found, attempting to create compiled model at " << compiledModelPath
                  << std::endl;

        Ort::ModelCompilationOptions compile_options(env, sessionOptions);
        compile_options.SetInputModelPath(modelPath.c_str());
        compile_options.SetOutputModelPath(compiledModelPath.c_str());

        std::cout << "Starting compile, this may take a few moments..." << std::endl;
        Ort::Status compileStatus = Ort::CompileModel(env, compile_options);
        if (compileStatus.IsOK())
        {
            // Calculate the duration in minutes / seconds / milliseconds
            std::cout << "Model compiled successfully!" << std::endl;
            isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);
        }
        else
        {
            std::cerr << "Failed to compile model: " << compileStatus.GetErrorCode() << ", "
                      << compileStatus.GetErrorMessage() << std::endl;
            std::cerr << "Falling back to uncompiled model" << std::endl;
        }
    }
    std::filesystem::path modelPathToUse = isCompiledModelAvailable ? compiledModelPath : modelPath;

    // Create the session and load the model
    Ort::Session session(env, modelPathToUse.c_str(), sessionOptions);
    std::cout << "ResNet model loaded"<< std::endl;

    // Load and Preprocess image
    winrt::hstring imagePath{ dogImagePath.c_str()};
    auto imageFrameResult = ResnetModelHelper::LoadImageFileAsync(imagePath);
    auto inputTensorData = ResnetModelHelper::BindSoftwareBitmapAsTensor(imageFrameResult.get());

    // Prepare input tensor
    auto inputInfo = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
    auto inputType = inputInfo.GetElementType();

    auto inputShape = std::array<int64_t, 4>{ 1, 3, 224, 224 };
    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::vector<uint8_t> rawInputBytes;

    if (inputType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16)
    {
        auto converted = ResnetModelHelper::ConvertFloat32ToFloat16(inputTensorData);
        rawInputBytes.assign(reinterpret_cast<uint8_t*>(converted.data()),
            reinterpret_cast<uint8_t*>(converted.data()) + converted.size() * sizeof(uint16_t));
    }
    else
    {
        rawInputBytes.assign(reinterpret_cast<uint8_t*>(inputTensorData.data()),
            reinterpret_cast<uint8_t*>(inputTensorData.data()) +
            inputTensorData.size() * sizeof(float));
    }

    OrtValue* ortValue = nullptr;

    Ort::ThrowOnError(Ort::GetApi().CreateTensorWithDataAsOrtValue(memoryInfo, rawInputBytes.data(),
        rawInputBytes.size(), inputShape.data(),
        inputShape.size(), inputType, &ortValue));
    Ort::Value inputTensor{ ortValue };

    const int iterations = 20;
    std::cout << "Running inference for " << iterations << " iterations" << std::endl;
	auto before = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iterations; i++)
	{
        //std::cout << "---------------------------------------------" << std::endl;
		//std::cout << "Running inference for " << i + 1 << "th time" << std::endl;
		//std::cout << "---------------------------------------------"<< std::endl;
        std::cout << ".";
        
        // Get input/output names
        Ort::AllocatorWithDefaultOptions allocator;
        auto inputName = session.GetInputNameAllocated(0, allocator);
        auto outputName = session.GetOutputNameAllocated(0, allocator);
        std::vector<const char*> inputNames = {inputName.get()};
        std::vector<const char*> outputNames = {outputName.get()};

        // Run inference
        auto outputTensors =
            session.Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, 1, outputNames.data(), 1);

        // Extract results
        std::vector<float> results;
        if (inputType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16)
        {
            auto outputData = outputTensors[0].GetTensorMutableData<uint16_t>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            std::vector<uint16_t> outputFloat16(outputData, outputData + outputSize);
            results = ResnetModelHelper::ConvertFloat16ToFloat32(outputFloat16);
        }
        else
        {
            auto outputData = outputTensors[0].GetTensorMutableData<float>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            results.assign(outputData, outputData + outputSize);
        }

        if (i == iterations - 1)
        {
            // Load labels and print result
            std::cout << "\nOutput for the last iteration"<< std::endl;
            auto labels = ResnetModelHelper::LoadLabels(labelsPath);
            ResnetModelHelper::PrintResults(labels, results);
        }
        inputName.release();
        outputName.release();
	}
    std::cout << "---------------------------------------------" << std::endl;
    auto after = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    std::cout << "Time taken for " << iterations << " iterations: " << duration.count() / 1000ull << " seconds" << std::endl;
    std::cout << "Avg time per iteration : "<< duration.count() / static_cast<long>(iterations) << " milliseconds" << std::endl;


    return 0;
}
