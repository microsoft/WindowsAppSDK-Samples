# CppConsoleDesktop.FrameworkDependent

This is a framework-dependent variant of the WindowsML C++ console desktop sample that uses shared helpers.

## Key Differences from Original

- **Deployment**: Framework-dependent (`WindowsAppSDKSelfContained=false`)
- **Package Dependencies**: Uses both ML and Runtime packages for framework-dependent deployment
- **Shared Helpers**: Uses the same shared helper classes from `../../Shared/cpp/`
- **Source Files**: References the main application source from `../CppConsoleDesktop/CppConsoleDesktop.cpp`
- **Runtime Dependencies**: Requires WindowsAppSDK Runtime to be installed on target machine

## Features

- Framework-dependent deployment with smaller application footprint
- Relies on WindowsAppSDK Runtime package installed on target machine
- Shared runtime components reduce deployment size
- Follows the recommended deployment pattern for WindowsAppSDK applications

## Required NuGet Package References

- Microsoft.WindowsAppSDK.ML
- Microsoft.WindowsAppSDK.Runtime  
- Microsoft.WindowsAppSDK.Base
- Microsoft.WindowsAppSDK.Foundation
- Microsoft.WindowsAppSDK.InteractiveExperiences
- Microsoft.Windows.CppWinRT
- Microsoft.Windows.SDK.BuildTools
- Microsoft.Windows.SDK.BuildTools.MSIX

See `packages.config` for current package versions.

## Windows App SDK Bootstrap Configuration

Framework-dependent applications require bootstrap initialization to properly discover and initialize the Windows App SDK runtime at startup. This project is configured to automatically include the necessary bootstrap code.

### How It Works

1. **Foundation Package**: The `Microsoft.WindowsAppSDK.Foundation` package provides bootstrap functionality for framework-dependent deployments.

2. **InteractiveExperiences Dependency**: Foundation package depends on `Microsoft.WindowsAppSDK.InteractiveExperiences` which provides essential UI metadata (like `Microsoft.UI.WindowId`) required by CppWinRT projections.

3. **Bootstrap Property**: The project sets `<WindowsAppSdkBootstrapInitialize>true</WindowsAppSdkBootstrapInitialize>` **before** importing any Windows App SDK packages.

4. **Auto-Generated Code**: When the property is set correctly, the Foundation package automatically includes `MddBootstrapAutoInitializer.cpp` in the build, which handles runtime discovery and initialization.

### Key Configuration Requirements

- The `WindowsAppSdkBootstrapInitialize` property must be defined **before** importing Windows App SDK packages
- The Foundation package `.props` and `.targets` files must be imported in the correct order
- The InteractiveExperiences package is required as a dependency of Foundation
- Framework-dependent projects (`WindowsAppSDKSelfContained=false`) require this bootstrap mechanism

### Manual Configuration Steps

If setting up a new framework-dependent project:

1. Add the required packages to `packages.config`:
   ```xml
   <package id="Microsoft.WindowsAppSDK.Foundation" version="[version]" targetFramework="native" />
   <package id="Microsoft.WindowsAppSDK.InteractiveExperiences" version="[version]" targetFramework="native" />
   <package id="Microsoft.WindowsAppSDK.Runtime" version="[version]" targetFramework="native" />
   <package id="Microsoft.WindowsAppSDK.Base" version="[version]" targetFramework="native" />
   ```

2. Set the bootstrap property at the top of your `.vcxproj` file (before any imports):
   ```xml
   <PropertyGroup>
     <WindowsAppSdkBootstrapInitialize>true</WindowsAppSdkBootstrapInitialize>
   </PropertyGroup>
   ```

3. Import Foundation and InteractiveExperiences package build files:
   ```xml
   <!-- In props section -->
   <Import Project="packages\Microsoft.WindowsAppSDK.Foundation.[version]\build\native\Microsoft.WindowsAppSDK.Foundation.props" />
   <Import Project="packages\Microsoft.WindowsAppSDK.InteractiveExperiences.[version]\build\native\Microsoft.WindowsAppSDK.InteractiveExperiences.props" />
   
   <!-- In targets section -->
   <Import Project="packages\Microsoft.WindowsAppSDK.Foundation.[version]\build\native\Microsoft.WindowsAppSDK.Foundation.targets" />
   <Import Project="packages\Microsoft.WindowsAppSDK.InteractiveExperiences.[version]\build\native\Microsoft.WindowsAppSDK.InteractiveExperiences.targets" />
   ```

### Common Issues and Solutions

- **CppWinRT Error: "Microsoft.UI.WindowId could not be found"**
  - **Solution**: Ensure the InteractiveExperiences package is included. Foundation depends on UI metadata provided by this package.
  
- **Bootstrap initialization not working**
  - **Solution**: Verify the `WindowsAppSdkBootstrapInitialize` property is set to `true` **before** any package imports in the vcxproj file.

- **Duplicate file warnings during build**
  - **Note**: Multiple packages may include the same auto-initializer files. The linker will automatically ignore duplicates - these warnings are informational only.

This configuration ensures the Windows App SDK runtime is properly initialized when your application starts, even when the runtime is not packaged with your application.

## Deployment Requirements

The target machine must have the Windows App SDK Runtime installed. This is the recommended deployment model for most applications as it allows for shared runtime components and easier servicing.
