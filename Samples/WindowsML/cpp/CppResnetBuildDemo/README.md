# Windows ML walkthrough (C++)

This short tutorial walks through using Windows ML to run the ResNet-50 image classification model on Windows using C++, detailing model acquisition and preprocessing steps. The implementation involves dynamically selecting execution providers for optimized inference performance.

The ResNet-50 model is a PyTorch model intended for image classification.

In this tutorial, you'll acquire the ResNet-50 model and convert it to QDQ ONNX format by using the AI Toolkit.

Then you'll load the model, prepare input tensors, and run inference using the Windows ML APIs with C++, including post-processing steps to apply softmax, and retrieve the top predictions.

## Acquiring the model, and preprocessing

You can acquire [ResNet-50](https://huggingface.co/microsoft/resnet-50) and convert ResNet-50 to QDQ ONNX format by using the AI Toolkit (see [convert models to ONNX format](https://code.visualstudio.com/docs/intelligentapps/modelconversion) for more info).

The goal of this example code is to leverage the Windows ML runtime to do the heavy lifting.

The Windows ML runtime will:

* Load the model.
* Dynamically select the preferred IHV-provided execution provider (EP) for the model and download its EP from the Microsoft Store, on demand.
* Run inference on the model using the EP.

For API reference, see [**OrtCompileApi struct**](https://onnxruntime.ai/docs/api/c/struct_ort_api.html), [**OrtSessionOptions**](https://onnxruntime.ai/docs/api/c/group___global.html#gaa6c56bcb36e39611481a17065d3ce620), [**Microsoft::Windows::AI::MachineLearning::Infrastructure class**](./api-reference.md#infrastructure-class), and [**Ort::GetApi**](https://onnxruntime.ai/docs/api/c/namespace_ort.html#a296b5958479d9889218b17bdb08c1894).

```cpp
#include <winml/onnxruntime_cxx_api.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

// Create ONNX Runtime environment
auto env = Ort::Env();

// Use WinML to download and register Execution Providers
auto catalog = winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
auto providers = catalog.FindAllProviders();
for (const auto& provider : providers)
{
    provider.EnsureReadyAsync().get();
    provider.TryRegister();
}

// Set the auto EP selection policy
Ort::SessionOptions sessionOptions;
sessionOptions.SetEpSelectionPolicy(OrtExecutionProviderDevicePolicy_MIN_OVERALL_POWER);
```

### EP compilation

If your model isn't already compiled for the EP (which may vary depending on device), the model might need to be compiled against that EP. This is a one-time process. The example code below handles it by compiling the model on the first run, and then storing it locally. Subsequent runs of the code pick up the compiled version, and run that; resulting in optimized fast inferences.

For API reference, see [**Ort::ModelCompilationOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_model_compilation_options.html), [**Ort::Status struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_status.html), and [**Ort::CompileModel**](https://onnxruntime.ai/docs/api/c/namespace_ort.html#af5ec45452237ac4ab98dd7a11b9d678e).

```cpp
// Compile the model if not already compiled
std::filesystem::path compiledModelPath = L"compiled_model\\model.onnx";
bool isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);

if (!isCompiledModelAvailable)
{
    Ort::ModelCompilationOptions compile_options(env, sessionOptions);
    compile_options.SetInputModelPath(modelPath.c_str());
    compile_options.SetOutputModelPath(compiledModelPath.c_str());

    Ort::Status compileStatus = Ort::CompileModel(env, compile_options);
    if (compileStatus.IsOK())
    {
        isCompiledModelAvailable = std::filesystem::exists(compiledModelPath);
    }
}

std::filesystem::path modelPathToUse = isCompiledModelAvailable ? compiledModelPath : modelPath;
```

## Running the inference

The input image is converted to tensor data format, and then inference runs on it. While this is typical of all code that uses the ONNX Runtime, the difference in this case is that it's ONNX Runtime directly through Windows ML. The only requirement is adding `#include <winml/onnxruntime_cxx_api.h>` to the code.

Also see [Convert a model with AI Toolkit for VS Code](https://code.visualstudio.com/docs/intelligentapps/modelconversion)

For API reference, see [**Ort::Session struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_session.html), [**Ort::MemoryInfo struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_memory_info.html), [**Ort::Value struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_value.html), [**Ort::AllocatorWithDefaultOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_allocator_with_default_options.html), [**Ort::RunOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_run_options.html).

```cpp
// Create the session and load the model
Ort::Session session(env, modelPathToUse.c_str(), sessionOptions);

// Load and preprocess image
auto imageFrameResult = ResnetModelHelper::LoadImageFileAsync(imagePath);
auto inputTensorData = ResnetModelHelper::BindSoftwareBitmapAsTensor(imageFrameResult.get());

// Create input tensor (handle both float32 and float16 models)
auto inputType = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetElementType();
auto inputShape = std::array<int64_t, 4>{1, 3, 224, 224};
// ... convert tensor data based on model type and create OrtValue ...
Ort::Value inputTensor{ortValue};

// Run inference
auto inputName = session.GetInputNameAllocated(0, allocator);
auto outputName = session.GetOutputNameAllocated(0, allocator);
auto outputTensors = session.Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, 1, outputNames.data(), 1);

// Extract results (handle float16/float32 conversion if needed)
std::vector<float> results;
// ... extract and convert output tensor data ...

// Load labels and print results  
auto labels = ResnetModelHelper::LoadLabels(labelsPath);
ResnetModelHelper::PrintResults(labels, results);
```

### Image preprocessing

The C++ implementation includes comprehensive image preprocessing that handles image loading, resizing, and normalization:

```cpp
std::vector<float> BindSoftwareBitmapAsTensor(SoftwareBitmap const& bitmap)
{
    // Convert to BGRA8 format and resize to 224x224
    SoftwareBitmap bitmapBgra8 = SoftwareBitmap::Convert(bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore);
    // ... use BitmapEncoder/Decoder to resize with high-quality interpolation ...
    
    // Extract pixel data and normalize using ImageNet parameters
    // ... get pixel buffer access ...
    uint8_t* pixelData = /* ... */;
    
    std::vector<float> tensorData(3 * 224 * 224);
    for (int y = 0; y < 224; ++y)
    {
        for (int x = 0; x < 224; ++x)
        {
            // Extract BGR values and normalize with ImageNet mean/std
            float r = static_cast<float>(pixelData[idx + 2]) / 255.0f;
            float g = static_cast<float>(pixelData[idx + 1]) / 255.0f;
            float b = static_cast<float>(pixelData[idx + 0]) / 255.0f;

            tensorData[0 * 224 * 224 + offset] = (r - 0.485f) / 0.229f;
            tensorData[1 * 224 * 224 + offset] = (g - 0.456f) / 0.224f;
            tensorData[2 * 224 * 224 + offset] = (b - 0.406f) / 0.225f;
        }
    }
    return tensorData;
}
```

### Post-processing

The softmax function is applied to returned raw output, and label data is used to map and print the names with the five highest probabilities.

```cpp
void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results)
{
    // Apply softmax with numerical stability
    float maxLogit = *std::max_element(results.begin(), results.end());
    std::vector<float> softmaxResults;
    // ... compute exp(logit - maxLogit) and normalize ...

    // Get top 5 results and sort by confidence
    std::vector<std::pair<int, float>> indexedResults;
    // ... create index-value pairs and sort descending ...
    
    // Display formatted results
    std::cout << "Top Predictions:\n-------------------------------------------\n";
    std::cout << std::left << std::setw(32) << "Label" << std::right << std::setw(10) << "Confidence\n";
    std::cout << "-------------------------------------------\n";

    for (const auto& result : indexedResults)
    {
        std::cout << std::left << std::setw(32) << labels[result.first] << std::right << std::setw(10) 
                  << std::fixed << std::setprecision(2) << (result.second * 100) << "%\n";
    }
    std::cout << "-------------------------------------------\n";
}
```

### Output  

Here's an example of the kind of output to be expected.

```console
-------------------------------------------
Label                            Confidence
-------------------------------------------
French bulldog                       45.07%
bull mastiff                         35.05%
boxer                                 2.85%
pug                                   1.01%
American Staffordshire terrier        0.57%
-------------------------------------------
```
