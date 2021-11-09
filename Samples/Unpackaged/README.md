---
name: "Unpackaged App Sample"
description: "Demonstrate how to use the Windows App SDK in non-MSIX (unpackaged) application."
page_type: sample
languages:
- csharp
- cpp
products:
- windows
- windows-app-sdk
urlFragment: Unpackaged
---
# Use the Windows App SDK from non-MSIX deployed applications

These samples demonstrate how simple it is to use the Windows App SDK from a non-MSIX (unpackaged) application using the auto-intialization that is part of the WinAppSDK nuget package.

## List of Samples

- [Basic - C++](Basic/cpp-console-unpackaged) - Console app sample showing how to set a project property that initializes access to the Windows App SDK and call a simple API.
- [Basic - C#](Basic/cs-console-unpackaged) - Console app sample showing how to set a project property that initializes access the Windows App SDK and call a simple API.

## Scenarios covered in these samples

### Using the Windows App SDK in an unpackaged desktop app

When using the Windows App SDK in an unpackaged app, the developer needs to add a `PackageReference` the NuGet and set `<WindowsPackageType>None</WindowsPackageType>` in the project file. This property tells the build system to auto-initialize the framework package for use with the app using module initializers in both C# and C++. If developers want more control over the initialization, they can explicitly call the Bootstrapper APIs to initialize the framework package. See [https://docs.microsoft.com/windows/apps/windows-app-sdk/reference-framework-package-run-time](https://docs.microsoft.com/windows/apps/windows-app-sdk/reference-framework-package-run-time)

## Building and running any of the samples

**VS Developer Environment:** - Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

**Windows App SDK dependencies:** In addition to the steps for setting up the environment for the Windows App SDK, with unpackaged applications it is the developer's responsibility to ensure that the appropriate MSIX dependencies are installed prior to running the sample - follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

Once the dependencies are installed, now you can open the solution in VS and build:

1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio 2019 or 2022.
2. Press F5 to build and debug the solution.

## Related Links

- [MSIX framework packages and dynamic dependencies](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/framework-packages-overview)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
