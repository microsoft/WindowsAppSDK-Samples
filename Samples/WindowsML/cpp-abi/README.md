# Windows ML ABI Sample (C++)

This sample demonstrates Windows ML execution provider discovery and management using direct ABI/COM interfaces with the Microsoft.WindowsAppSDK.ML NuGet package. Unlike other samples that use C++/WinRT projections, this implementation provides low-level access to Windows Runtime interfaces through automatically generated ABI headers.

## Overview

The Microsoft.WindowsAppSDK.ML package provides execution provider APIs for hardware-accelerated machine learning inference. This sample shows how to:

- **Discover execution providers** using `IExecutionProviderCatalog` ABI interface
- **Manage provider lifecycle** through `IExecutionProvider` ABI interface
- **Handle async operations** with progress reporting and completion events
- **Generate ABI headers automatically** using Microsoft.Windows.AbiWinRT package

This approach is ideal for developers who need direct control over COM object lifetimes, want minimal runtime overhead, or are integrating with existing ABI-based codebases.

## Key Features

### Automatic ABI Header Generation
- **Microsoft.Windows.AbiWinRT 2.0**: Generates C++ ABI headers from WinRT metadata during build
- **Build-time generation**: Headers created in `$(IntDir)\Generated Files\` directory
- **Filtered output**: Only generates headers for specified metadata (Microsoft.Windows.AI.MachineLearning)

### Execution Provider Management
- **Provider discovery**: Enumerate available execution providers (CPU, GPU, NPU)
- **Async preparation**: Use `EnsureReadyAsync` with progress reporting
- **State management**: Track provider readiness (NotPresent, NotReady, Ready)
- **Registration**: Register providers with ONNX Runtime for inference

### Direct ABI Programming
- **Raw COM interfaces**: Direct access without C++/WinRT projection overhead
- **Microsoft::WRL**: Smart pointers and implementation helpers for COM objects
- **Event-driven async**: HANDLE-based synchronization with completion handlers
- **Manual lifetime management**: Explicit control over object creation and destruction

## When to Use This Approach

**Choose ABI interfaces when:**
- Integrating with existing C-style or ABI-based codebases
- Requiring precise control over COM object lifetimes
- Building low-level frameworks or middleware
- Minimizing runtime dependencies and projection overhead
- Working in environments where C++/WinRT projections are not available

## Project Structure

```
├── main.cpp                      # Application entry point
├── AbiUtils.h                     # HSTRING utilities and common helpers
├── AsyncHandlers.h/.cpp           # Progress and completion handlers
├── ExecutionProvider.h/.cpp       # IExecutionProvider wrapper
├── ExecutionProviderCatalog.h/.cpp# IExecutionProviderCatalog wrapper
├── ConsoleUI.h/.cpp              # User interface helpers
├── Application.h/.cpp            # Application orchestration
└── CppAbiEPEnumerationSample.vcxproj    # MSBuild project with ABI generation
```

## Build Configuration

### NuGet Package Dependencies
```xml
<PackageReference Include="Microsoft.WindowsAppSDK.ML"/>
<PackageReference Include="Microsoft.WindowsAppSDK.Foundation"/>
<PackageReference Include="Microsoft.WindowsAppSDK.InteractiveExperiences"/>
<PackageReference Include="Microsoft.WindowsAppSDK.Base"/>
<PackageReference Include="Microsoft.Windows.AbiWinRT"/>
```

### Automatic Header Generation
```xml
<PropertyGroup>
  <!-- Filter ABI generation to Windows ML APIs only -->
  <AbiWinRTFilter>Microsoft.Windows.AI.MachineLearning</AbiWinRTFilter>
  <!-- Windows SDK version for metadata resolution -->
  <WindowsSDKVersion>10.0.26100.0</WindowsSDKVersion>
</PropertyGroup>
```

The build process automatically generates these headers:
- `Microsoft.Windows.AI.MachineLearning.h` - Execution provider interfaces
- `Windows.Foundation.h` - Async operation and fundamental types

## Prerequisites

- **Windows 11** version 24H2 (build 26100) or later
- **Visual Studio 2022** with C++ development workload
- **Windows 11 SDK** version 10.0.26100.0 or later
- **Windows App SDK 1.8** or later runtime (automatically deployed in self-contained mode)

## Building and Running

### Visual Studio
1. Open `CppAbiEPEnumerationSample.vcxproj` in Visual Studio 2022
2. Select Debug/Release and x64 platform configuration
3. Build solution (F7) - ABI headers generate automatically during build
4. Run (F5) or run without debugging (Ctrl+F5)

### Command Line
```powershell
# Build project
msbuild CppAbiEPEnumerationSample.vcxproj /p:Configuration=Release /p:Platform=x64

# Run executable
.\x64\Release\CppAbiEPEnumerationSample.exe
```

### Expected Output
```
=== Windows ML Execution Provider Sample ===
Using C++ ABI/COM interfaces
===========================================

Available Execution Providers:
  1. SomeExecutionProvider [Not Ready]
     No description available

Select the provider (1), or 0 to exit: 1
Selected: VitisAIExecutionProvider
Provider current state: Not Ready
Calling SomeExecutionProvider to prepare the provider...
Progress: 45.2%
Progress: 78.9%
Progress: 100.0%
Provider state after EnsureReady: Ready
Provider registered successfully!
```
