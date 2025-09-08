# CppConsoleDesktop.SelfContained

This is a self-contained variant of the WindowsML C++ console desktop sample.

## Key Differences from Original

- **Deployment**: Self-contained (`WindowsAppSDKSelfContained=true`)
- **Package Dependencies**: Uses minimal package set with ML and Base packages for proper binary deployment
- **Shared Helpers**: Uses the same shared helper classes from `../../Shared/cpp/`
- **Source Files**: References the main application source from `../CppConsoleDesktop/CppConsoleDesktop.cpp`
- **Binary Deployment**: ONNX Runtime binaries are bundled alongside the application

## Required NuGet Package References

- Microsoft.WindowsAppSDK.ML
- Microsoft.WindowsAppSDK.Base
- Microsoft.WindowsAppSDK.Foundation
- Microsoft.WindowsAppSDK.InteractiveExperiences
- Microsoft.Windows.CppWinRT
- Microsoft.Windows.SDK.BuildTools
- Microsoft.Windows.SDK.BuildTools.MSIX

See `packages.config` for current package versions.