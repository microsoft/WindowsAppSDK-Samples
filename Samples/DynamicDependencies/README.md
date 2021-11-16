---
name: "Dynamic Dependencies Sample"
description: "Demonstrate how to use Dynamic Dependencies API to load MSIX framework packages in any application"
page_type: sample
languages:
- cpp
products:
- windows
- windows-app-sdk
urlFragment: DynamicDependencies
---

# Dynamically load MSIX framework packages from your application

Dynamic Dependencies is an API in the Windows App SDK that enables applications the ability to dynamically load MSIX framework packages. This sample demonstrates how to use those APIs to dynamically load the DirectX Runtime MSIX framework package.

## List of Samples

- [DirectX - C++](DirectX/) - DirectX sample (D3D9Ex), built using the older DirectX9 SDK, but updated to show how to use Dynamic Dependencies and the DirectX Framework package instead of the DirectX redist.

## Scenarios covered in these samples

### Dynamically referencing additional MSIX framework packages

This example also shows how to add a reference to another MSIX framework package (DirectX) by explicitly using the Dynamic Dependency API. For more information on these concepts, please refer to [https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/use-the-dynamic-dependency-API](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/use-the-dynamic-dependency-API)

## Building and running any of the samples

**Known issue:** With 1.0.0-preview3, you will see the following error:

*C1083 Cannot open include file: 'TerminalVelocityFeatures-DynamicDependency.h"*.

To workaround this error, open the file *MsixDynamicDependency.h.*, comment out `#include <TerminalVelocityFeatures-DynamicDependency.h>`, and add `#include <stdint.h>`.

**VS Developer Environment:** - Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

**Windows App SDK dependencies:** In addition to the steps for setting up the environment for the Windows App SDK, with unpackaged applications it is the developer's responsibility to ensure that the appropriate MSIX dependencies are installed prior to running the sample - follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

**DirectX SDK dependencies:** To Build and Run the DirectX sample, you need the both the [DirectX SDK](https://www.microsoft.com/download/details.aspx?id=6812) installed, and you need to download and install the architecture appropriate DirectX framework package (x86 or x64) - follow the instructions [here](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications).

Once the dependencies are installed, now you can open the solution in VS and build:
1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio 2019 or 2022.
2. Press F5 to build and debug the solution.

## Related Links

- [MSIX framework packages and dynamic dependencies](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/framework-packages-overview)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
- [DirectX framework packages](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications)
- [Legacy DirectX SDK download](https://www.microsoft.com/download/details.aspx?id=6812)
