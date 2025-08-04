# CppConsoleDesktop.SelfContained

This is a self-contained variant of the WindowsML C++ console desktop sample.

## Key Differences from Original

- **Deployment**: Self-contained (`WindowsAppSDKSelfContained=true`)
- **Package Dependencies**: Uses minimal package set with ML and Base packages for proper binary deployment
- **Source Files**: References the same source files from `../CppConsoleDesktop/CppConsoleDesktop/`
- **Binary Deployment**: ONNX Runtime binaries are bundled in a `winml\` subdirectory alongside the application

## Features

- Self-contained deployment with all ML dependencies bundled
- No external WindowsAppSDK runtime required on target machine
- ONNX Runtime binaries deployed locally for standalone operation
- Portable deployment that runs without framework dependencies

## Build

```powershell
# Restore packages first
nuget restore packages.config -PackagesDirectory packages

# Build the project
msbuild CppConsoleDesktop.SelfContained.vcxproj /p:Platform=x64 /p:Configuration=Release
```

> **Note**: If you encounter package restore issues in Visual Studio, use the command-line `nuget restore` as shown above.

## Package Dependencies

- WindowsAppSDK ML package (core machine learning functionality)
- WindowsAppSDK Base package (required for proper binary deployment)
- CppWinRT package (Windows Runtime projections)
- Windows SDK Build Tools packages (build-time dependencies)

See `packages.config` for current package versions.