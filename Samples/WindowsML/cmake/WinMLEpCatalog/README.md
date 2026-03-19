# Windows ML EP Catalog Sample (CMake/WinML C API)

This sample demonstrates the **WinMLEpCatalog WinML C API** for discovering and managing hardware-accelerated execution providers (EPs) for machine learning inference on Windows.

The CMake project automatically restores the `Microsoft.WindowsAppSDK.ML` NuGet package during configure (via `nuget install`) and extracts its CMake config for consumption.

## Overview

This sample showcases:

- Creating an ONNX Runtime environment (`Ort::Env`)
- Creating and releasing a catalog handle
- Enumerating registered execution providers
- Inspecting provider metadata (name, version, state, certification)
- Preparing providers with `EnsureReady`
- Registering prepared providers with ONNX Runtime

## Prerequisites

- **Visual Studio 2022** with C++ workload
- **CMake** 3.21 or later
- **NuGet** CLI (`nuget.exe` on PATH)
- **Ninja** (optional; only needed when using Ninja generator)

Example install commands:

```powershell
winget install --exact --id Kitware.CMake
winget install --exact --id Ninja-build.Ninja
```

## Build (Recommended)

Use the helper script:

```powershell
# RelWithDebInfo for host architecture
.\build.ps1

# Debug with Visual Studio generator
.\build.ps1 -Generator VisualStudio -Configuration Debug

# Release for ARM64
.\build.ps1 -Configuration Release -Platform arm64

# Clean and rebuild
.\build.ps1 -Clean
```

During configure, CMake automatically:

1. Looks for a local `.nupkg` in this directory (useful for pre-release testing)
2. If not found, runs `nuget install` to download the package from the repo's NuGet feed
3. Extracts to `out/build/<preset>/packages/` and sets `microsoft.windows.ai.machinelearning_DIR`

## Manual Build

```powershell
# Configure (auto-restores the NuGet package)
cmake --preset nuget

# Build
cmake --build out/build/nuget --config RelWithDebInfo

# Run
.\out\build\nuget\WinMLEpCatalogSample.exe
```

## Package Version Override

To use a different version:

```powershell
cmake --preset nuget -DWINML_NUGET_VERSION=1.8.2124
```

To use a local `.nupkg` instead of NuGet restore, place it in this directory
named `Microsoft.WindowsAppSDK.ML.<version>.nupkg`.

For a full refresh, delete `out/` and configure again.

## Troubleshooting

### "nuget install failed"

Ensure `nuget.exe` is on your PATH and that the package version is published.

### "Execution provider registration failed"

Check that `onnxruntime_providers_shared.dll` and `DirectML.dll` were copied to the executable output directory and the target OS is supported.

## License

Copyright (C) Microsoft Corporation. All rights reserved.
