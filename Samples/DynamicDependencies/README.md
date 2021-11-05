# Use the Windows App SDK from non-MSIX deployed applications

Dynamic Dependencies is an api call that allows unpackaged applications (non-MSIX deployed apps) the ability to load MSIX framework packages. These samples demonstrated how to use Dynamic Dependencies and call APIs in the Windows App SDK and other MSIX framework packages, such as the DirectX Runtime.

## List of Samples

- [Basic - C++](Basic/cpp-console-unpackaged) - Console app sample showing how to initialize the bootstrapper to access the Windows App SDK and call a simple api.
- [Basic - C#](Basic/cs-console-unpackaged) - Console app sample showing how to initialize the managed bootstrapper component to access the Windows App SDK and call a simple api.
- [DirectX - C++](DirectX/) - DirectX sample (D3D9Ex), built using the older DirectX9 SDK, but updated to show how to use Dynamic Dependencies and the DirectX Framework package instead of the DirectX redist.

## Scenarios covered in these samples
### Using the Windows App SDK in an unpackaged desktop app
When using the WinAppSDK in an unpackaged app, the developer needs to add a packagereference the nuget and set WindowsPackageType=None in the project properties. This property auto-initializes the framework package for use with application. If developers want more control over the initialization, they can explicitly call the Bootstrapper APIs to initialize the framework package. See [https://docs.microsoft.com/windows/apps/windows-app-sdk/reference-framework-package-run-time](https://docs.microsoft.com/windows/apps/windows-app-sdk/reference-framework-package-run-time)


### Dynamically referencing additional MSIX framework packages
This example also shows how to add a reference to another MSIX framework package (DirectX) by explicitly using the Dynamic Dependency api. For more information on these concepts, please refer to [https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/use-the-dynamic-dependency-api](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/use-the-dynamic-dependency-api)

## Building and running any of the samples

**VS Developer Environment:** - Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

**Windows App SDK dependencies:** In addition to the steps for setting up the environment for the Windows App SDK, with unpackaged applications it is the developer's responsibility to ensure that the appropriate MSIX dependencies are installed prior to running the sample - follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

**DirectX SDK dependencies:** To Build and Run the DirectX sample, you need the both the [DirectX SDK](https://www.microsoft.com/download/details.aspx?id=6812) installed, and you need to download and install the architecture appropriate DirectX framework package (x86 or x64) - follow the instructions [here](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications)

Once the dependencies are installed, now you can open the solution in VS and build:
1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio 2019 or 2022.
2. Press F5 to build and debug the solution.

## Related Links

- [MSIX framework packages and dynamic dependencies](https://docs.microsoft.com/windows/apps/desktop/modernize/framework-packages/framework-packages-overview)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
- [DirectX framework packages](https://docs.microsoft.com/windows/win32/dxtecharts/directx-setup-for-game-developers#desktop-bridge-applications)
- [Legacy DirectX SDK download](https://www.microsoft.com/download/details.aspx?id=6812)