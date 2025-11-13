# C# Console Desktop Sample for ONNX Runtime

This sample demonstrates how to use ONNX Runtime in a C# desktop application, focusing on:

1. Execution Provider (EP) discovery and configuration
2. Model compilation for optimized inference
3. Command-line options for flexible usage

## Command-line Usage

```shell
CSharpConsoleDesktop.exe [options]
Options:
  --ep_policy <policy>          Set execution provider policy (NPU, CPU, GPU, DEFAULT, DISABLE). Default: DISABLE
  --compile                     Compile the model
  --download                    Download required packages
  --model <path>                Path to input ONNX model (default: SqueezeNet.onnx in executable directory)
  --compiled_output <path>      Path for compiled output model (default: SqueezeNet_ctx.onnx)
  --image_path <path>           Path to the input image (default: sample kitten image)
```

## Key Features

### 1. Execution Provider Configuration

The sample demonstrates how to discover available execution providers and configure them:

```csharp
using Microsoft.ML.OnnxRuntime;
using WindowsML.Shared;

// Initialize execution providers using shared helper  
await ModelManager.InitializeExecutionProvidersAsync(options.Download);

// Create inference session with EP configuration using shared helper
using InferenceSession session = ModelManager.CreateSession(actualModelPath, options, ortEnv);
```

The ExecutionProviderManager handles the discovery and configuration of execution providers:

```csharp
// Get all available EP devices from the environment
IReadOnlyList<OrtEpDevice> epDevices = environment.GetEpDevices();

Console.WriteLine($"Discovered {epDevices.Count} execution provider device(s)");

// Accumulate devices by ep_name
Dictionary<string, List<OrtEpDevice>> epDeviceMap = new(StringComparer.OrdinalIgnoreCase);

// Group devices by EP name
foreach (OrtEpDevice device in epDevices)
{
    string epName = device.EpName;
    if (!epDeviceMap.ContainsKey(epName))
    {
        epDeviceMap[epName] = [];
    }
    epDeviceMap[epName].Add(device);
}

// Configure execution providers
foreach (KeyValuePair<string, List<OrtEpDevice>> epGroup in epDeviceMap)
{
    string epName = epGroup.Key;
    List<OrtEpDevice> devices = epGroup.Value;

    // Configure EP with all its devices
    Dictionary<string, string> epOptions = new(StringComparer.OrdinalIgnoreCase);

    switch (epName)
    {
        case "VitisAIExecutionProvider":
            sessionOptions.AppendExecutionProvider(environment, devices, epOptions);
            Console.WriteLine($"Successfully added {epName} EP");
            break;
        // ... other providers
    }
}
```

### 2. Model Compilation

The sample shows how to compile an ONNX model for optimized execution:

```csharp
// Create compilation options from session options
OrtModelCompilationOptions compileOptions = new(sessionOptions);

try
{
    // Set input and output model paths
    compileOptions.SetInputModelPath(modelPath);
    compileOptions.SetOutputModelPath(compiledModelPath);

    Console.WriteLine("Starting compile, this may take a few moments...");
    DateTime start = DateTime.Now;

    // Compile the model
    compileOptions.CompileModel();

    TimeSpan duration = DateTime.Now - start;
    Console.WriteLine($"Model compiled successfully in {duration.TotalMilliseconds} ms");
}
catch
{
    Console.Error.WriteLine($"Failed to compile model, continuing ...");
}
```

### 3. Execution Provider Selection Policy

The sample demonstrates how to set an EP selection policy to prefer specific hardware:

```csharp
// Prefer NPU if available
SessionOptions sessionOptions = new();
sessionOptions.SetEpSelectionPolicy(ExecutionProviderDevicePolicy.PREFER_NPU);
```

The sample supports the following execution provider policies:

- **NPU**: Prefer Neural Processing Unit
- **CPU**: Prefer CPU execution
- **GPU**: Prefer GPU execution
- **DEFAULT**: Use default provider selection