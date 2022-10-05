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

These samples demonstrate how to author Windows Widgets with WindowsAppSDK.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
    * For the unpackaged sample apps, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## SingleWidget and MultpleWidgets sample

Inside this sample folder you will find 2 different projects:
- Sample project that demonstrates authoring of only 1 widget within the same app.
- Sample project that demonstrates authoring >=1 widgets within the same app.
  
To get familiar with the implementation steps and API details it's recommended to start with the single widget authoring sample.

Once the widgets ideology and implementation steps are understood, you may examine the MultipleWidgets sample to get an idea on how to create an application that will be able to serve multiple widgets at a time.

Relevant documentation and full walkthrough of a similar sample may be found [here](https://learn.microsoft.com/en-us/windows/apps/develop/widgets/implement-widget-provider-win32).

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Learn more about Windows Widgets](https://learn.microsoft.com/en-us/windows/apps/design/widgets/)
- [Implement a Win32 widget provider](https://learn.microsoft.com/en-us/windows/apps/develop/widgets/implement-widget-provider-win32)
