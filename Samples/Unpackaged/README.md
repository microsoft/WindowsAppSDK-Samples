---
page_type: sample
languages:
- csharp
- cpp
products:
- windows
- windows-app-sdk
name: "Unpackaged App Sample"
urlFragment: Unpackaged
description: "Demonstrate how to use the Windows App SDK in non-MSIX (unpackaged) application."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Use the Windows App SDK from non-MSIX deployed applications

These samples demonstrate how simple it is to use the Windows App SDK from a non-MSIX (unpackaged) application using the auto-intialization that is part of the Windows App SDK NuGet package.

## List of Samples

- [Basic - C++](cpp-console-unpackaged) - Console app sample showing how to set a project property that initializes access to the Windows App SDK and call a simple API.
- [Basic - C#](cs-console-unpackaged) - Console app sample showing how to set a project property that initializes access the Windows App SDK and call a simple API.

## Scenarios covered in these samples

### Using the Windows App SDK in an unpackaged desktop app

When using the Windows App SDK in an unpackaged app, the developer needs to add a `PackageReference` to the Windows App SDK NuGet and set `<WindowsPackageType>None</WindowsPackageType>` in the project file. This property tells the build system to auto-initialize the framework package for use with the app using module initializers in both C# and C++. If developers want more control over the initialization, they can explicitly call the Bootstrapper APIs to initialize the framework package. See [Referencing the Windows App SDK framework package at run time](https://docs.microsoft.com/windows/apps/windows-app-sdk/reference-framework-package-run-time).

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* Since these samples are unpackaged, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps) to ensure that the appropriate MSIX dependencies are installed.

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [MSIX framework packages and dynamic dependencies](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/framework-packages-overview)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
