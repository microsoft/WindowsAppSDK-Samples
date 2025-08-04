# CppConsoleDesktop.FrameworkDependent

This is a framework-dependent variant of the WindowsML C++ console desktop sample.

## Key Differences from Original

- **Deployment**: Framework-dependent (`WindowsAppSDKSelfContained=false`)
- **Package Dependencies**: Uses both ML and Runtime packages for framework-dependent deployment
- **Source Files**: References the same source files from `../CppConsoleDesktop/CppConsoleDesktop/`
- **Runtime Dependencies**: Requires WindowsAppSDK Runtime to be installed on target machine

## Features

- Framework-dependent deployment with smaller application footprint
- Relies on WindowsAppSDK Runtime package installed on target machine
- Shared runtime components reduce deployment size
- Follows the recommended deployment pattern for WindowsAppSDK applications

## Build

```powershell
# Restore packages first
nuget restore packages.config -PackagesDirectory packages

# Build the project
msbuild CppConsoleDesktop.FrameworkDependent.vcxproj /p:Platform=x64 /p:Configuration=Release
```

> **Note**: If you encounter package restore issues in Visual Studio, use the command-line `nuget restore` as shown above.

## Package Dependencies

- WindowsAppSDK ML package (core machine learning functionality)
- WindowsAppSDK Runtime package (framework runtime components)
- WindowsAppSDK Base package (required for proper binary deployment)
- CppWinRT package (Windows Runtime projections)
- Windows SDK Build Tools packages (build-time dependencies)

See `packages.config` for current package versions.

## Deployment Requirements

The target machine must have the Windows App SDK Runtime installed. This is the recommended deployment model for most applications as it allows for shared runtime components and easier servicing.
