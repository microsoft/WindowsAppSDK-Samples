---
page_type: sample
languages:
- csharp
- cpp
products:
- windows
- windows-app-sdk
name: "Windowing gallery sample"
urlFragment: windowing
description: "Shows how to use the Windows App SDK windowing APIs"
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Windowing gallery sample

These samples demonstrate how to use the AppWindow class to manage your application's window.

## Limitations

Please note that customization of a window's TitleBar using the AppWindowTitleBar APIs is only supported on Windows 11 or later. This is illustrated in the sample by using the (IsCustomizationSupported)[https://docs.microsoft.com/windows/windows-app-sdk/api/winrt/microsoft.ui.windowing.appwindowtitlebar.iscustomizationsupported] method. Please see the [Title bar
  customization](https://docs.microsoft.com/windows/apps/develop/title-bar) article for more details.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
    * For the unpackaged sample apps, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).
* The C# samples require either .NET SDK 5.0.205 or later, or .NET SDK 5.0.400 or later.

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Manage app windows](https://docs.microsoft.com/windows/apps/windows-app-sdk/windowing/windowing-overview)
