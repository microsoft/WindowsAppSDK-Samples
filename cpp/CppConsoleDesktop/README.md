# C++ Console Desktop Sample for ONNX Runtime

This sample demonstrates how to use ONNX Runtime in a C++ desktop application, focusing on:

1. Execution Provider (EP) discovery and configuration
2. Model compilation for optimized inference
3. Command-line options for flexible usage

## Build

### Prerequisites

- Visual Studio 2022 with C++ development tools
- Windows SDK (10.0.22621.3233 or later)
- NuGet CLI (for command-line builds)

### Building with Visual Studio

1. Open `CppConsoleDesktop.sln` in Visual Studio 2022
2. Select your desired configuration (Debug/Release) and platform (x64/ARM64)
3. Build the solution using **Build > Build Solution** or `Ctrl+Shift+B`

### Building from Command Line

```powershell
# Restore NuGet packages (from the README directory)
nuget restore CppConsoleDesktop/packages.config -PackagesDirectory packages

# Alternatively, restore packages from within the project subdirectory
cd CppConsoleDesktop
nuget restore packages.config -PackagesDirectory ../packages
cd ..

# Build the project (x64 Release)
msbuild CppConsoleDesktop/CppConsoleDesktop.vcxproj /p:Platform=x64 /p:Configuration=Release

# Or build for ARM64
msbuild CppConsoleDesktop/CppConsoleDesktop.vcxproj /p:Platform=ARM64 /p:Configuration=Release
```

> **Note**: If you encounter package restore issues in Visual Studio, use the command-line `nuget restore` as shown above.

## Command-line Usage

```
CppConsoleDesktop.exe [options]
Options:
  --ep_policy <policy>  Set execution provider policy (NPU, CPU, GPU, DEFAULT, DISABLE). Default: DISABLE
  --compile            Compile the model
  --download           Download required packages
  --model <path>       Path to input ONNX model (default: SqueezeNet.onnx in executable directory)
  --compiled_output <path>      Path for compiled output model (default: SqueezeNet_ctx.onnx)
  --image_path <path>           Path to the input image (default: image.jpg in the executable directory)
```

## Package Dependencies

This sample uses the complete Microsoft WindowsAppSDK package suite, including:

- **Microsoft.WindowsAppSDK** - Core Windows App SDK functionality
- **Microsoft.WindowsAppSDK.ML** - Machine learning and ONNX Runtime integration
- **Microsoft.WindowsAppSDK.AI** - AI capabilities and execution providers
- **Microsoft.WindowsAppSDK.Runtime** - Runtime components and base services  
- **Microsoft.WindowsAppSDK.Foundation** - Foundation libraries
- **Microsoft.WindowsAppSDK.WinUI** - WinUI 3 framework components
- **Microsoft.WindowsAppSDK.Widgets** - Windows Widgets functionality
- **Microsoft.WindowsAppSDK.InteractiveExperiences** - Interactive experiences
- **Microsoft.WindowsAppSDK.DWrite** - DirectWrite text rendering
- **Microsoft.WindowsAppSDK.Base** - Base deployment components
- **Microsoft.Windows.CppWinRT** - C++/WinRT language projections
- **Microsoft.Windows.SDK.BuildTools** - Windows SDK build tools
- **Microsoft.Web.WebView2** - WebView2 control support

See `CppConsoleDesktop\packages.config` for specific package versions.

## Deployment

This sample is configured as a self-contained application that includes all necessary WindowsAppSDK components. No additional runtime installation is required on the target machine.

## Key Features

### 1. Execution Provider Configuration

The sample demonstrates how to discover available execution providers and configure them:

```cpp
#include <winml/onnxruntime_cxx_api.h>

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

// Configure and add each EP with appropriate options
for (const auto& [ep_name, devices] : ep_device_map)
{
    Ort::KeyValuePairs ep_options;
    if (ep_name == "VitisAIExecutionProvider")
    {
        // Append the Vitis AI EP
        session_options.AppendExecutionProvider_V2(env, devices, ep_options);
        std::cout << "Successfully added " << ep_name << " EP" << std::endl;
    }
    // ... other providers
}
```

### 2. Model Compilation

The sample shows how to compile an ONNX model for optimized execution:

```cpp
#include <winml/onnxruntime_cxx_api.h>

// Get compile API
const OrtCompileApi* compileApi = ortApi.GetCompileApi();

// Create compilation options
OrtModelCompilationOptions* compileOptions = nullptr;
compileApi->CreateModelCompilationOptionsFromSessionOptions(env, sessionOptions, &compileOptions);

// Set input and output paths
compileApi->ModelCompilationOptions_SetInputModelPath(compileOptions, modelPath.c_str());
compileApi->ModelCompilationOptions_SetOutputModelPath(compileOptions, compiledModelPath.c_str());

// Compile the model
compileApi->CompileModel(env, compileOptions);
```

### 3. Execution Provider Selection Policy

The sample demonstrates how to set an EP selection policy to prefer specific hardware:

```cpp
#include <winml/onnxruntime_cxx_api.h>

// Prefer NPU if available
Ort::SessionOptions sessionOptions;
sessionOptions.SetEpSelectionPolicy(OrtExecutionProviderDevicePolicy_PREFER_NPU);
```
