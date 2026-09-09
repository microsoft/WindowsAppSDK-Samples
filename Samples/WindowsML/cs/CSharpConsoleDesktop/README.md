# C# Console Desktop Sample for ONNX Runtime

This sample demonstrates how to use ONNX Runtime in a C# desktop application, focusing on:

1. Execution Provider (EP) discovery and configuration
2. Model compilation for optimized inference
3. Command-line options for flexible usage

## Command-line Usage

```shell
CSharpConsoleDesktop.exe [options]
Options:
  --ep_policy <policy>           (Required*) Set execution provider selection policy (NPU, CPU, GPU, DEFAULT)
  --ep_name <name>               (Required*) Explicit execution provider name (mutually exclusive with --ep_policy)
  --compile                      Compile the model
  --download                     Download required packages
  --model <path>                 Path to input ONNX model (default: SqueezeNet.onnx in executable directory)
  --compiled_output <path>       Path for compiled output model (default: auto-generated with device info)
  --image_path <path>            Path to the input image (default: sample kitten image)

Exactly one of --ep_policy or --ep_name must be specified.
--use_model_catalog and --model are mutually exclusive.
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

The sample shows how to compile an ONNX model for optimized execution. Compiled model filenames
are automatically generated with device-specific identifiers to prevent collisions:

- Policy mode: `SqueezeNet_ctx_PREFER_GPU.onnx`
- Explicit EP: `SqueezeNet_ctx_DML_GPU.onnx`
- With perf mode: `SqueezeNet_ctx_PREFER_NPU_MaxPerformance.onnx`

Use `--compiled_output` to override with a custom path.

Before an existing compiled model is reused, the shared `ModelManager` reads its EP compatibility
metadata and validates it against the currently discovered devices. Explicit `--ep_name` and
`--device_type` selections are honored. With `--ep_policy`, devices are grouped by EP and each EP
group is probed separately.

Only `EP_SUPPORTED_OPTIMAL` is reused. `EP_SUPPORTED_PREFER_RECOMPILATION`, `EP_UNSUPPORTED`,
`EP_NOT_APPLICABLE`, discovery failures, and validation failures cause the sample to use the
original ONNX model instead. For policy selection, at least one candidate EP group must have
matching metadata, and every metadata-bearing candidate group must report `EP_SUPPORTED_OPTIMAL`.
Candidate groups follow policy fallback behavior: CPU for `DEFAULT`/`PREFER_CPU`, NPU plus CPU for
NPU/efficiency policies, and GPU plus CPU for GPU/performance policies. If no candidate group has
matching metadata, the cache is also treated as non-optimal.

When `--compile` is present and the cached model is missing or non-optimal, compilation writes to a
temporary file in the output directory and replaces the cache only after compilation succeeds. If
compilation or replacement fails, a stale compiled model is never selected and inference falls
back to the original model. EP context data is embedded in the compiled ONNX file so atomic cache
replacement does not leave references to temporary sidecar files.

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