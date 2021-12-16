---
page_type: sample
languages:
- cpp
products:
- windows
- windows-app-sdk
name: "Dynamic Dependencies Sample"
urlFragment: DynamicDependencies
description: "Demonstrate how to use Dynamic Dependencies API to load MSIX framework packages in any application"
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Dynamically load MSIX framework packages from your application

Dynamic Dependencies is an API in the Windows App SDK that enables applications the ability to dynamically load MSIX framework packages. This sample demonstrates how to use those APIs to dynamically load the DirectX Runtime MSIX framework package.

The [C++ DirectX sample](DirectX/) (D3D9Ex) is built using the older DirectX9 SDK, but updated to show how to use Dynamic Dependencies and the DirectX Framework package instead of the DirectX redist.

## Scenarios covered in these samples

### Dynamically referencing additional MSIX framework packages

This sample shows how to add a reference to another MSIX framework package (DirectX) by explicitly using the Dynamic Dependency API. For more information on these concepts, please refer to [Use the dynamic dependency API to reference framework packages at run time.](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/use-the-dynamic-dependency-API)

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* Since the sample is unpackaged, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps) to ensure that the appropriate MSIX dependencies are installed.
* **DirectX SDK dependencies:** To build and run the DirectX sample, you need to download and install the following:
    * [DirectX SDK](https://www.microsoft.com/download/details.aspx?id=6812)
    * [DirectX framework package](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications): choose the architecture-appropriate version (x86 or x64)

## Building and running the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [MSIX framework packages and dynamic dependencies](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/framework-packages-overview)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
- [DirectX framework packages](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications)
- [Legacy DirectX SDK download](https://www.microsoft.com/download/details.aspx?id=6812)
