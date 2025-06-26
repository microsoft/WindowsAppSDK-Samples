# Windows ML walkthrough

This short tutorial walks through using Windows ML to run the ResNet-50 image classification model on Windows, detailing model acquisition and preprocessing steps. The implementation involves dynamically selecting execution providers for optimized inference performance.

The ResNet-50 model is a PyTorch model intended for image classification.

In this tutorial, you'll acquire the ResNet-50 model and convert it to QDQ ONNX format by using the AI Toolkit.

Then you'll load the model, prepare input tensors, and run inference using the Windows ML APIs, including post-processing steps to apply softmax, and retrieve the top predictions.

## Acquiring the model, and preprocessing

You can acquire [ResNet-50](https://huggingface.co/microsoft/resnet-50) and convert ResNet-50 to QDQ ONNX format by using the AI Toolkit (see [convert models to ONNX format](https://code.visualstudio.com/docs/intelligentapps/modelconversion) for more info).

The goal of this example code is to leverage the Windows ML runtime to do the heavy lifting.

The Windows ML runtime will:
* Load the model.
* Dynamically select the preferred IHV-provided execution provider (EP) for the model and download its EP from the Microsoft Store, on demand.
* Run inference on the model using the EP.

For API reference, see [**OrtCompileApi struct**](https://onnxruntime.ai/docs/api/c/struct_ort_api.html), [**OrtSessionOptions**](https://onnxruntime.ai/docs/api/c/group___global.html#gaa6c56bcb36e39611481a17065d3ce620), [**Microsoft::Windows::AI::MachineLearning::Infrastructure class**](./api-reference.md#infrastructure-class), and [**Ort::GetApi**](https://onnxruntime.ai/docs/api/c/namespace_ort.html#a296b5958479d9889218b17bdb08c1894).

```csharp
// Create a new instance of EnvironmentCreationOptions
EnvironmentCreationOptions envOptions = new()
{
    logId = "ResnetBuildDemo",
    logLevel = OrtLoggingLevel.ORT_LOGGING_LEVEL_ERROR
};

// Pass the options by reference to CreateInstanceWithOptions
OrtEnv ortEnv = OrtEnv.CreateInstanceWithOptions(ref envOptions);

// Use WinML to download and register Execution Providers
Microsoft.Windows.AI.MachineLearning.Infrastructure infrastructure = new();
Console.WriteLine("Ensure EPs are downloaded ...");
await infrastructure.DownloadPackagesAsync();
await infrastructure.RegisterExecutionProviderLibrariesAsync();

//Create Onnx session
Console.WriteLine("Creating session ...");
var sessionOptions = new SessionOptions();
// Set EP Selection Policy
sessionOptions.SetEpSelectionPolicy(ExecutionProviderDevicePolicy.MIN_OVERALL_POWER);
```

### EP compilation

If your model isn't already compiled for the EP (which may vary depending on device), the model might need to be compiled against that EP. This is a one-time process. The example code below handles it by compiling the model on the first run, and then storing it locally. Subsequent runs of the code pick up the compiled version, and run that; resulting in optimized fast inferences.

For API reference, see [**Ort::ModelCompilationOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_model_compilation_options.html), [**Ort::Status struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_status.html), and [**Ort::CompileModel**](https://onnxruntime.ai/docs/api/c/namespace_ort.html#af5ec45452237ac4ab98dd7a11b9d678e).

```csharp
// Compile the model if not already compiled
string compiledModelPath = @"C:\Build\compiled_model\model.onnx";
bool isCompiled = File.Exists(compiledModelPath);
if (!isCompiled)
{
    Console.WriteLine("No compiled model found. Compiling model ...");
    using (var compileOptions = new OrtModelCompilationOptions(sessionOptions))
    {
        compileOptions.SetInputModelPath(modelPath);
        compileOptions.SetOutputModelPath(compiledModelPath);
        compileOptions.CompileModel();
        isCompiled = File.Exists(compiledModelPath);
        if (isCompiled)
        {
            Console.WriteLine("Model compiled successfully!");
        }
        else
        {
            Console.WriteLine("Failed to compile the model. Will use original model.");
        }
    }
}
else
{
    Console.WriteLine("Found precompiled model.");
}
var modelPathToUse = isCompiled ? compiledModelPath : modelPath;
```

## Running the inference

The input image is converted to tensor data format, and then inference runs on it. While this is typical of all code that uses the ONNX Runtime, the difference in this case is that it's ONNX Runtime directly through Windows ML. The only requirement is adding `#include <win_onnxruntime_cxx_api.h>` to the code.

Also see [Convert a model with AI Toolkit for VS Code](https://code.visualstudio.com/docs/intelligentapps/modelconversion)

For API reference, see [**Ort::Session struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_session.html), [**Ort::MemoryInfo struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_memory_info.html), [**Ort::Value struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_value.html), [**Ort::AllocatorWithDefaultOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_allocator_with_default_options.html), [**Ort::RunOptions struct**](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_run_options.html).

```csharp
using var session = new InferenceSession(modelPathToUse, sessionOptions);

Console.WriteLine("Preparing input ...");
// Load and preprocess image
var input = await PreprocessImageAsync(await LoadImageFileAsync(imagePath));
// Prepare input tensor
var inputName = session.InputMetadata.First().Key;
var inputTensor = new DenseTensor<float>(
    input.ToArray(),          // Use the DenseTensor<float> directly
    new[] { 1, 3, 224, 224 }, // Shape of the tensor
    false                     // isReversedStride should be explicitly set to false
);

// Bind inputs and run inference
var inputs = new List<NamedOnnxValue>
{
    NamedOnnxValue.CreateFromTensor(inputName, inputTensor)
};

Console.WriteLine("Running inference ...");
var results = session.Run(inputs);
for (int i = 0; i < 40; i++)
{
    results = session.Run(inputs);
}

// Extract output tensor
var outputName = session.OutputMetadata.First().Key;
var resultTensor = results.First(r => r.Name == outputName).AsEnumerable<float>().ToArray();

// Load labels and print results
var labels = LoadLabels(labelsPath);
PrintResults(labels, resultTensor);
```

### Post-processing.

The softmax function is applied to returned raw output, and label data is used to map and print the names with the five highest probabilities.

```csharp
private static void PrintResults(IList<string> labels, IReadOnlyList<float> results)
{
    // Apply softmax to the results
    float maxLogit = results.Max();
    var expScores = results.Select(r => MathF.Exp(r - maxLogit)).ToList(); // stability with maxLogit
    float sumExp = expScores.Sum();
    var softmaxResults = expScores.Select(e => e / sumExp).ToList();

    // Get top 5 results
    IEnumerable<(int Index, float Confidence)> topResults = softmaxResults
        .Select((value, index) => (Index: index, Confidence: value))
        .OrderByDescending(x => x.Confidence)
        .Take(5);

    // Display results
    Console.WriteLine("Top Predictions:");
    Console.WriteLine("-------------------------------------------");
    Console.WriteLine("{0,-32} {1,10}", "Label", "Confidence");
    Console.WriteLine("-------------------------------------------");

    foreach (var result in topResults)
    {
        Console.WriteLine("{0,-32} {1,10:P2}", labels[result.Index], result.Confidence);
    }

    Console.WriteLine("-------------------------------------------");
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
