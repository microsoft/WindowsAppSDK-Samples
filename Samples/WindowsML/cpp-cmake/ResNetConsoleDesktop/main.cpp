#include "ResNetModelHelper.hpp"

#include <ranges>
#include <winml/onnxruntime_c_api.h>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>
#include <winrt/base.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <winml/onnxruntime_cxx_api.h>
#include <winml/Runtime.h>

#include <WindowsAppSDK-VersionInfo.h>
#include <MddBootstrap.h>

using namespace ResNetModelHelper;
int wmain(int argc, wchar_t* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;

    try
    {
        std::cout << WINDOWSAPPSDK_RELEASE_MAJORMINOR << std::endl;

        const UINT32 c_majorMinorVersion{WINDOWSAPPSDK_RELEASE_MAJORMINOR};
        PCWSTR c_versionTag{WINDOWSAPPSDK_RELEASE_VERSION_TAG_W};
        const PACKAGE_VERSION c_minVersion{WINDOWSAPPSDK_RUNTIME_VERSION_UINT64};
        const auto c_options{MddBootstrapInitializeOptions_OnNoMatch_ShowUI};
        const HRESULT hr{::MddBootstrapInitialize2(c_majorMinorVersion, c_versionTag, c_minVersion, c_options)};
        if (FAILED(hr))
        {
            exit(hr);
        }

        // Initialize WinML runtime first. This will add the necessary package dependencies to the process, and initialize the OnnxRuntime.
        Microsoft::Windows::AI::MachineLearning::WinMLRuntime winmlRuntime;
        if (FAILED(winmlRuntime.GetHResult()))
        {
            std::cerr << "Failed to initialize WinML runtime: " << std::hex << winmlRuntime.GetHResult() << std::endl;
            return -1;
        }

        winrt::init_apartment();
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "CppConsoleDesktop");

        // Use WinML to download and register Execution Providers
        winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog catalog =
            winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
        auto providers = catalog.FindAllProviders();
        for (const auto& provider : providers)
        {
            std::wcout << L"Provider            : " << std::wstring_view{provider.Name()} << L'\n';
            std::wcout << L" ReadyState         : " << provider.ReadyState() << L'\n';
            winrt::Windows::Foundation::IAsyncOperationWithProgress action = provider.EnsureReadyAsync();

            action.Progress([](const auto& sender, double progress) {
                std::wcout << L" Progress      : " << progress << L"%\n";
            });

            winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyResult result = action.get();
            std::wcout << L" ReadyResultState   : " << result.Status() << L'\n';
            std::wcout << L" ExtendedError      : 0x" << std::hex << result.ExtendedError() << L'\n';
            std::wcout << L" DiagnosticText     : " << result.DiagnosticText() << L'\n';

            winrt::hresult errorCode = action.ErrorCode();
            std::wcout << L" ErrorCode          : " << errorCode << L'\n';

            const bool registerResult = provider.TryRegister();
            std::wcout << L" registerResult     : " << registerResult << L'\n';
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
            const auto* outputData = outputTensors[0].GetTensorData<uint16_t>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            std::vector<uint16_t> outputFloat16(std::from_range, std::span{outputData, outputSize});
            results = ResNetModelHelper::ConvertFloat16ToFloat32(outputFloat16);
        }
        else
        {
            const auto* outputData = outputTensors[0].GetTensorData<float>();
            size_t outputSize = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            results = std::vector{std::from_range, std::span{outputData, outputSize}};
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
