#include "ResNetModelHelper.hpp"
#include "winml/onnxruntime_c_api.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>
#include <winrt/base.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

#include <winml/onnxruntime_cxx_api.h>

int wmain(int argc, wchar_t* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;

    try
    {
        winrt::init_apartment();
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "CppConsoleDesktop");

        // Use WinML to download and register Execution Providers
        winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog catalog =
            winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
        auto providers = catalog.FindAllProviders();
        for (const auto& provider : providers)
        {
            std::wcout << L"Provider    : " << std::wstring_view{provider.Name()} << L'\n';
            std::wcout << L" ReadyState : " << std::to_underlying(provider.ReadyState()) << L'\n';
            winrt::Windows::Foundation::IAsyncOperationWithProgress action = provider.EnsureReadyAsync();

            action.Progress([](const auto& sender, double progress) {
                std::wcout << L"  Progress  : " << progress << L"%\n";
            });

            action.get();
            provider.TryRegister();
        }

        std::vector<Ort::ConstEpDevice> devices = env.GetEpDevices();
        std::wcout << L"ONNX providers registered: \n";
        for (const Ort::ConstEpDevice& device : devices)
        {
            std::cout << device.EpName() << "\n";
        }

        // Set the auto EP selection policy
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetEpSelectionPolicy(OrtExecutionProviderDevicePolicy_PREFER_CPU);

        // Prepare paths for model and labels
        const std::filesystem::path executableFolder = ResNetModelHelper::GetExecutablePath().parent_path();
        const std::filesystem::path modelPath = executableFolder / L"model.onnx";
        const std::filesystem::path labelsPath = executableFolder / L"model.Labels.txt";
        const std::filesystem::path inputImagePath = executableFolder / L"dog.jpg";
        const std::filesystem::path compiledModelPath = executableFolder / L"model_ctx.onnx";

        bool isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);
        if (isCompiledModelAvailable)
        {
            std::wcout << L"Using compiled model: " << compiledModelPath.wstring() << L'\n';
        }
        else
        {
            std::wcout << L"No compiled model found, attempting to create compiled model at " << compiledModelPath.wstring() << L'\n';

            Ort::ModelCompilationOptions compile_options(env, sessionOptions);
            compile_options.SetInputModelPath(modelPath.c_str());
            compile_options.SetOutputModelPath(compiledModelPath.c_str());

            std::wcout << L"Starting compile, this may take a few moments...\n";
            Ort::Status compileStatus = Ort::CompileModel(env, compile_options);
            if (compileStatus.IsOK())
            {
                // Calculate the duration in minutes / seconds / milliseconds
                std::wcout << L"Model compiled successfully!\n";
                isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);
            }
            else
            {
                std::cerr << "Failed to compile model: " << compileStatus.GetErrorCode() << ", "
                          << compileStatus.GetErrorMessage() << std::endl;
                std::wcerr << "Falling back to uncompiled model\n";
            }
        }
        std::filesystem::path modelPathToUse = isCompiledModelAvailable ? compiledModelPath : modelPath;

        // Create the session and load the model
        Ort::Session session(env, modelPathToUse.c_str(), sessionOptions);

        // Load and Preprocess image
        winrt::hstring imagePath{inputImagePath.c_str()};

        auto inputInfo = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
        auto inputType = inputInfo.GetElementType();

        auto imageFrameResult = ResNetModelHelper::LoadImageFileAsync(imagePath);
        auto inputTensorData = ResNetModelHelper::BindSoftwareBitmapAsTensor(imageFrameResult.get());

        auto inputShape = std::array<int64_t, 4>{1, 3, 224, 224};
        auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        std::vector<uint8_t> rawInputBytes;

        if (inputType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16)
        {
            auto converted = ResNetModelHelper::ConvertFloat32ToFloat16(inputTensorData);
            rawInputBytes.assign(
                reinterpret_cast<uint8_t*>(converted.data()),
                reinterpret_cast<uint8_t*>(converted.data()) + converted.size() * sizeof(uint16_t));
        }
        else
        {
            rawInputBytes.assign(
                reinterpret_cast<uint8_t*>(inputTensorData.data()),
                reinterpret_cast<uint8_t*>(inputTensorData.data()) + inputTensorData.size() * sizeof(float));
        }

        OrtValue* ortValue = nullptr;

        Ort::ThrowOnError(
            Ort::GetApi().CreateTensorWithDataAsOrtValue(
                memoryInfo, rawInputBytes.data(), rawInputBytes.size(), inputShape.data(), inputShape.size(), inputType, &ortValue));
        Ort::Value inputTensor{ortValue};

        // Get input/output names
        Ort::AllocatorWithDefaultOptions allocator;
        auto inputName = session.GetInputNameAllocated(0, allocator);
        auto outputName = session.GetOutputNameAllocated(0, allocator);
        std::array inputNames = {inputName.get()};
        std::array outputNames = {outputName.get()};

        // Run inference
        auto outputTensors = session.Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, 1, outputNames.data(), 1);

        // Extract results
        std::vector<float> results;
        if (inputType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16)
        {
            auto outputData = outputTensors[0].GetTensorMutableData<uint16_t>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            std::vector<uint16_t> outputFloat16(outputData, outputData + outputSize);
            results = ResNetModelHelper::ConvertFloat16ToFloat32(outputFloat16);
        }
        else
        {
            auto outputData = outputTensors[0].GetTensorMutableData<float>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            results.assign(outputData, outputData + outputSize);
        }

        // Load labels and print result
        auto labels = ResNetModelHelper::LoadLabels(labelsPath);
        ResNetModelHelper::PrintResults(labels, results);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}
