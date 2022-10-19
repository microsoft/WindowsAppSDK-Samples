---
page_type: sample
languages:
- cpp
- cppwinrt
products:
- windows
- windows-app-sdk
name: "Windows Widgets Samples"
urlFragment: Widgets
description: "Shows how to author Windows Widgets with WindowsAppSDK"
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Windows Widgets samples

These samples demonstrate how to author Windows Widgets with the Windows App SDK.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
    * For the unpackaged sample apps, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## WidgetProvider sample

This project includes a sample of a 3rd party widget provider. At this point, it's only possible to author widgets in C++ and Widgetprovider application must be a packaged application.
The widget provider can be distributed in 2 ways:
- With the Developer Mode turned on on the machine you can sideload the packaged widget provider application.
- Distrubute it through Microsoft Store.

Relevant documentation and full walkthrough of a similar sample may be found [here](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32).

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Learn more about Windows Widgets](https://learn.microsoft.com/windows/apps/design/widgets/)
- [Implement a Win32 widget provider](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32)
