// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <MemoryBuffer.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <win_onnxruntime_cxx_api.h>
#include <windows.h>
#include <winrt/base.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;

// Command-line options structure
struct CommandLineOptions
{
    bool compile_model = false;
    bool download_packages = false;
    std::optional<OrtExecutionProviderDevicePolicy> ep_policy;
    std::wstring image_path;
    std::wstring model_path;
    std::wstring output_path;
};

// Parse command line arguments
bool ParseCommandLineArgs(int argc, wchar_t* argv[], CommandLineOptions& options)
{
    std::vector<std::wstring_view> arguments{argv, argv + argc};

    if (arguments.size() < 1)
    {
        std::wcout
            << L"Usage: CppConsoleDesktop.exe [options]\n"
            << L"Options:\n"
            << L"  --ep_policy <policy>  Set execution provider policy (NPU, CPU, GPU, DEFAULT, DISABLE). Default: "
               L"DISABLE\n"
            << L"  --compile                     Compile the model\n"
            << L"  --download                    Download required packages\n"
            << L"  --model <path>                Path to the input ONNX model (default: SqueezeNet.onnx in executable directory)\n"
            << L"  --compiled_output <path>      Path for compiled output model (default: SqueezeNet_ctx.onnx)\n"
            << L"  --image_path <path>           Path to the input image (default: image.jpg in the executable directory)\n";
        return false;
    }

    for (size_t i = 1; i < arguments.size(); ++i)
    {
        if (arguments[i] == L"--compile")
        {
            options.compile_model = true;
        }
        else if (arguments[i] == L"--download")
        {
            options.download_packages = true;
        }
        else if (arguments[i] == L"--model" && i + 1 < arguments.size())
        {
            options.model_path = arguments[++i];
        }
        else if (arguments[i] == L"--compiled_output" && i + 1 < arguments.size())
        {
            options.output_path = arguments[++i];
        }
        else if (arguments[i] == L"--image_path" && i + 1 < arguments.size())
        {
            options.image_path = arguments[++i];
        }
        else if (arguments[i] == L"--ep_policy" && i + 1 < arguments.size())
        {
            auto policy_str = arguments[++i];
            if (policy_str == L"NPU")
            {
                options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_NPU;
            }
            else if (policy_str == L"CPU")
            {
                options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_CPU;
            }
            else if (policy_str == L"GPU")
            {
                options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_GPU;
            }
            else if (policy_str == L"DEFAULT")
            {
                options.ep_policy = OrtExecutionProviderDevicePolicy_DEFAULT;
            }
            else if (policy_str == L"DISABLE")
            {
                options.ep_policy = std::nullopt;
            }
            else
            {
                std::wcout << L"Unknown EP policy: " << policy_str << L", using default (DISABLE)\n";
            }
        }
    }

    return true;
}

std::string ToString(OrtExecutionProviderDevicePolicy policy)
{
    switch (policy)
    {
    case OrtExecutionProviderDevicePolicy_PREFER_NPU:
        return "PREFER_NPU";
    case OrtExecutionProviderDevicePolicy_PREFER_CPU:
        return "PREFER_CPU";
    case OrtExecutionProviderDevicePolicy_PREFER_GPU:
        return "PREFER_GPU";
    case OrtExecutionProviderDevicePolicy_DEFAULT:
        return "DEFAULT";
    default:
        return "UNKNOWN";
    }
}

std::string ToString(OrtHardwareDeviceType policy)
{
    switch (policy)
    {
    case OrtHardwareDeviceType_NPU:
        return "NPU";
    case OrtHardwareDeviceType_CPU:
        return "CPU";
    case OrtHardwareDeviceType_GPU:
        return "GPU";
    default:
        return "";
    }
}

/**
 * @brief Configure execution providers by discovering and adding available EPs
 *
 * This function demonstrates how to:
 * 1. Discover available execution provider devices
 * 2. Configure them with provider-specific options
 * 3. Add them to the session options
 *
 * @param session_options Ort session options to configure
 * @param env Ort environment
 */
void ConfigureExecutionProviders(Ort::SessionOptions& session_options, Ort::Env& env)
{
    // Get all available EP devices from the environment
    std::vector<Ort::ConstEpDevice> ep_devices = env.GetEpDevices();

    std::cout << "Discovered " << ep_devices.size() << " execution provider device(s)" << std::endl;

    // Accumulate devices by ep_name
    // Passing all devices for a given EP in a single call allows the execution provider
    // to select the best configuration or combination of devices, rather than being limited
    // to a single device. This enables optimal use of available hardware if supported by the EP.
    std::unordered_map<std::string, std::vector<Ort::ConstEpDevice>> ep_device_map;
    for (const auto& device : ep_devices)
    {
        ep_device_map[device.EpName()].push_back(device);
    }
    std::wcout << L"Execution Provider Information:\n";
    std::wcout << L"-------------------------------------------------------------\n";
    std::wcout << std::left << std::setw(32) << L"Provider" << std::setw(16) << L"Vendor" << std::setw(12)
               << L"Device Type" << std::endl;
    std::wcout << L"-------------------------------------------------------------\n";
    for (const auto& [ep_name, devices] : ep_device_map)
    {
        std::cout << std::left << std::setw(32) << ep_name;
        for (const auto& device : devices)
        {
            std::cout << std::setw(16) << device.EpVendor() << std::setw(12) << ToString(device.Device().Type())
                       << std::endl;
        }
    }

    std::wcout << L"-------------------------------------------------------------\n";


    // Configure and append each EP type only once, with all its devices
    for (const auto& [ep_name, devices] : ep_device_map)
    {
        Ort::KeyValuePairs ep_options;
        if (ep_name == "VitisAIExecutionProvider")
        {
            // Append the Vitis AI EP
            session_options.AppendExecutionProvider_V2(env, devices, ep_options);
            std::cout << "Successfully added " << ep_name << " EP" << std::endl;
        }
        else if (ep_name == "OpenVINOExecutionProvider")
        {
            // Configure threading for OpenVINO EP, pick the first device found.
            ep_options.Add("num_of_threads", "4");
            session_options.AppendExecutionProvider_V2(env, {devices.front()}, ep_options);
            std::cout << "Successfully added " << ep_name << " EP (first device only)" << std::endl;
        }
        else if (ep_name == "QNNExecutionProvider")
        {
            // Configure performance mode for QNN EP
            ep_options.Add("htp_performance_mode", "high_performance");
            session_options.AppendExecutionProvider_V2(env, devices, ep_options);
            std::cout << "Successfully added " << ep_name << " EP" << std::endl;
        }
        else if (ep_name == "NvTensorRTRTXExecutionProvider")
        {
            // Add any specific options for the NVIDIA TensorRT RTX EP if needed
            session_options.AppendExecutionProvider_V2(env, devices, ep_options);
            std::cout << "Successfully added " << ep_name << " EP" << std::endl;
        }
        else
        {
            std::cout << "Skipping EP: " << ep_name << std::endl;
        }
    }
}

/**
 * @brief Compile an ONNX model using the OrtCompileApi
 *
 * This function demonstrates how to:
 * 1. Get the compile API
 * 2. Create compilation options from session options
 * 3. Configure input and output paths
 * 4. Compile the model
 *
 * @param ortApi ORT API instance
 * @param env ORT environment
 * @param sessionOptions Session options to use for compilation
 * @param modelPath Path to the input model
 * @param compiledModelPath Path where the compiled model will be saved
 * @return OrtStatus* Status of the compilation, nullptr if successful
 */
OrtStatus* CompileModel(const OrtApi& ortApi, Ort::Env& env, OrtSessionOptions* sessionOptions,
                        const std::filesystem::path& modelPath, const std::filesystem::path& compiledModelPath)
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
    OrtStatus* status =
        compileApi->CreateModelCompilationOptionsFromSessionOptions(env, sessionOptions, &compileOptions);
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

static std::filesystem::path GetModulePath(HMODULE module)
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

static std::filesystem::path GetExecutablePath()
{
    return GetModulePath(nullptr);
}

static IAsyncOperation<VideoFrame> LoadImageFileAsync(winrt::hstring filePath)
{
    StorageFile file = co_await StorageFile::GetFileFromPathAsync(filePath);
    auto stream = co_await file.OpenAsync(FileAccessMode::Read);
    BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(stream);
    SoftwareBitmap softwareBitmap = co_await decoder.GetSoftwareBitmapAsync();
    VideoFrame inputImage = VideoFrame::CreateWithSoftwareBitmap(softwareBitmap);

    co_return inputImage;
}

static std::vector<float> BindVideoFrameAsTensor(const VideoFrame& frame)
{
    SoftwareBitmap bitmap = frame.SoftwareBitmap();
    SoftwareBitmap bitmapBgra8 = SoftwareBitmap::Convert(bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore);

    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream = InMemoryRandomAccessStream();

    BitmapEncoder encoder = BitmapEncoder::CreateAsync(BitmapEncoder::BmpEncoderId(), stream).get();
    encoder.SetSoftwareBitmap(bitmapBgra8);
    encoder.BitmapTransform().ScaledWidth(224);
    encoder.BitmapTransform().ScaledHeight(224);
    encoder.BitmapTransform().InterpolationMode(BitmapInterpolationMode::Fant);
    encoder.FlushAsync().get();

    stream.Seek(0);
    BitmapDecoder decoder = BitmapDecoder::CreateAsync(stream).get();
    SoftwareBitmap resizedBitmap =
        decoder.GetSoftwareBitmapAsync(BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore).get();

    BitmapBuffer bitmapBuffer = resizedBitmap.LockBuffer(BitmapBufferAccessMode::Read);

    IMemoryBufferReference reference = bitmapBuffer.CreateReference();

    auto spByteAccess = reference.as<::Windows::Foundation::IMemoryBufferByteAccess>();
    uint8_t* pixelData = nullptr;
    uint32_t pixelDataCapacity = 0;

    spByteAccess->GetBuffer(&pixelData, &pixelDataCapacity);

    const int64_t channels = 3; // RGB
    const int64_t height = 224;
    const int64_t width = 224;

    std::vector<float> tensorData(channels * height * width);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = (y * width + x) * 4; // BGRA stride
            int offset = y * width + x;

            // The required input size is (3x224x224),
            // normalized using mean=[0.485, 0.456, 0.406]
            // and std=[0.229, 0.224, 0.225]
            float r = static_cast<float>(pixelData[idx + 2]) / 255.0f;
            float g = static_cast<float>(pixelData[idx + 1]) / 255.0f;
            float b = static_cast<float>(pixelData[idx + 0]) / 255.0f;

            tensorData[0 * height * width + offset] = (r - 0.485f) / 0.229f;
            tensorData[1 * height * width + offset] = (g - 0.456f) / 0.224f;
            tensorData[2 * height * width + offset] = (b - 0.406f) / 0.225f;
        }
    }

    return tensorData;
}

static void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results) {
    // Apply softmax to the results  
    float maxLogit = *std::max_element(results.begin(), results.end());
    std::vector<float> expScores;
    float sumExp = 0.0f;

    for (float r : results) {
        float expScore = std::exp(r - maxLogit);
        expScores.push_back(expScore);
        sumExp += expScore;
    }

    std::vector<float> softmaxResults;
    for (float e : expScores) {
        softmaxResults.push_back(e / sumExp);
    }

    // Get top 5 results  
    std::vector<std::pair<int, float>> indexedResults;
    for (size_t i = 0; i < softmaxResults.size(); ++i) {
        indexedResults.emplace_back(static_cast<int>(i), softmaxResults[i]);
    }

    std::sort(indexedResults.begin(), indexedResults.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });

    indexedResults.resize(std::min<size_t>(5, indexedResults.size()));

    // Display results  
    std::cout << "Top Predictions:\n";
    std::cout << "-------------------------------------------\n";
    std::cout << std::left << std::setw(32) << "Label" << std::right << std::setw(10) << "Confidence\n";
    std::cout << "-------------------------------------------\n";

    for (const auto& result : indexedResults) {
        std::cout << std::left << std::setw(32) << labels[result.first]
            << std::right << std::setw(10) << std::fixed << std::setprecision(2) << (result.second * 100) << "%\n";
    }

    std::cout << "-------------------------------------------\n";
}

static std::vector<std::string> LoadLabels(const std::filesystem::path& labelsPath)
{
    std::ifstream labelFile{labelsPath, std::ifstream::in};
    if (labelFile.fail())
    {
        throw std::runtime_error("Unable to load file.");
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

int wmain(int argc, wchar_t* argv[]) noexcept
{
    try
    {
        // Parse command line options
        CommandLineOptions options;
        if (!ParseCommandLineArgs(argc, argv, options))
        {
            return 0; // Help was displayed
        }

        winrt::init_apartment();

        // Create ONNX Runtime environment
        // This should be done before registering any provider libraries
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "CppConsoleDesktop");

        // The Infrastructure class is used to acquire and register execution provider libraries
        winrt::Microsoft::Windows::AI::MachineLearning::Infrastructure infrastructure;

        // Use WinML to update packages if requested
        if (options.download_packages)
        {
            std::cout << "Downloading packages..." << std::endl;
            infrastructure.DownloadPackagesAsync().get();
        }

        // Always register execution provider libraries
        infrastructure.RegisterExecutionProviderLibrariesAsync().get();

        std::filesystem::path executableFolder = GetExecutablePath().parent_path();

        // Set default model paths if not specified
        if (options.model_path.empty())
        {
            options.model_path = executableFolder / "SqueezeNet.onnx";
        }

        if (options.output_path.empty() && options.compile_model)
        {
            auto model_path = options.model_path;
            auto pos = model_path.rfind(L".");
            if (pos != std::wstring::npos)
            {
                options.output_path = std::wstring(model_path.substr(0, pos)) + L"_ctx.onnx";
            }
            else
            {
                options.output_path = model_path + L"_ctx.onnx";
            }
        }
        else if (options.output_path.empty())
        {
            auto model_path = options.model_path;
            auto pos = model_path.rfind(L".");
            if (pos != std::wstring::npos)
            {
                options.output_path = std::wstring(model_path.substr(0, pos)) + L"_ctx.onnx";
            }
            else
            {
                options.output_path = model_path + L"_ctx.onnx";
            }
        }

        if (options.image_path.empty())
        {
            options.image_path = executableFolder / L"image.jpg";
        }
        else
        {
            std::filesystem::path imagePath = options.image_path;
            if (!std::filesystem::exists(imagePath))
            {
                std::wcerr << L"Image file does not exist: " << imagePath << std::endl;
                return -1;
            }
        }
        std::wcout << "Image path: " << options.image_path  << std::endl;

        std::cout << "Creating session ..." << std::endl;

        std::filesystem::path modelPath = options.model_path;
        std::filesystem::path compiledModelPath = options.output_path;
        std::filesystem::path labelsPath = executableFolder / "SqueezeNet.Labels.txt";

        // Initialize the session options based on the EP policy
        // A CPU fallback will always be used if no EP is available
        Ort::SessionOptions sessionOptions;
        if (options.ep_policy.has_value())
        {
            // Set the EP selection policy based on command line
            std::cout << "Using EP Selection Policy: " << ToString(options.ep_policy.value()) << std::endl;
            sessionOptions.SetEpSelectionPolicy(options.ep_policy.value());
        }
        else
        {
            // Use explicit configuration
            std::cout << "Using explicit EP configuration" << std::endl;
            ConfigureExecutionProviders(sessionOptions, env);
        }

        // Handle model compilation if needed
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
            OrtStatus* status = CompileModel(ortApi, env, sessionOptions, modelPath, compiledModelPath);

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

        // Create the session and load the model
        std::cout << "Loading model: " << actualModelPath << std::endl;
        Ort::Session session(env, actualModelPath.c_str(), sessionOptions);

        // Get model input details
        Ort::AllocatorWithDefaultOptions allocator;
        auto inputName = session.GetInputNameAllocated(0, allocator);
        auto inputTypeInfo = session.GetInputTypeInfo(0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> inputShape = inputTensorInfo.GetShape();

        // For dynamic dimensions, set them to fixed values
        for (size_t i = 0; i < inputShape.size(); i++)
        {
            if (inputShape[i] == -1)
            {
                inputShape[i] = 1; // Assume batch size of 1
            }
        }

        // Get output name
        auto outputName = session.GetOutputNameAllocated(0, allocator);
        std::cout << "Preparing input ..." << std::endl;

        // Load and preprocess the image
        auto frame = LoadImageFileAsync(winrt::hstring{options.image_path}).get();
        auto labels = LoadLabels(labelsPath);

        // Create input tensor
        std::vector<float> inputTensorValues = BindVideoFrameAsTensor(frame);
        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputShape.data(), inputShape.size());

        // Prepare run inputs/outputs
        const char* inputNames[] = {inputName.get()};
        const char* outputNames[] = {outputName.get()};

        // Run inference
        std::cout << "Running inference ..." << std::endl;
        auto outputTensors = session.Run(Ort::RunOptions{nullptr}, inputNames, &inputTensor, 1, outputNames, 1);

        // Process results
        float* outputData = outputTensors[0].GetTensorMutableData<float>();
        const size_t outputCount = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
        std::vector<float> results(outputData, outputData + outputCount);

        // Print top 5 results
        PrintResults(labels, results);

        return 0;
    }
    catch (const Ort::Exception& ex)
    {
        std::cerr << "ONNX Runtime error: " << ex.what() << "\n";
    }
    catch (const winrt::hresult_error& error)
    {
        std::wcerr << L"Error: " << static_cast<std::wstring_view>(error.message()) << L"\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception!\n";
    }

    return -1;
}
