---
page_type: sample
languages:
- cpp
products:
- windows
- windows-app-sdk
- windows-api-win32
name: "Installer launch sample"
urlFragment: installer
description: "Shows how to use CreateProcess to launch the installer without a console window."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Installer launch sample

This sample demonstrates how to launch the Windows App SDK installer, **WindowsAppRuntimeInstall.exe**, without having a pop-up
window appearing while it is running. This sample is intended for Win32 programs which have setup
programs that need to embed the Windows App SDK without any pop-up windows.

## Prerequisites

* This sample requires downloading **WindowsAppRuntimeInstall.exe** from the Installer and MSIX packages in [Downloads for the Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/downloads).
* This sample assumes that **WindowsAppRuntimeInstall.exe** will be in %USERPROFILE%\Downloads for convenience.
If a different path is desired, it can be specified in the #define at the top in main.cpp.
* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running any of the samples

* Download **WindowsAppRuntimeInstall.exe** from the Installer and MSIX packages in [Downloads for the Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/downloads) or build a version of it, and place it in your local
`%USERPROFILE%\Downloads` folder..
* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

*Caution, this sample will execute whatever executable is at the specified path. It is recommended
to build the Installer with test packages from the WindowsAppSDK project or use a simple "Hello,
World" console application as the executable to avoid modifying the system with the sample.*

## Related Links

- [Creating Processes](https://docs.microsoft.com/windows/win32/procthread/creating-processes)
- [Process Creation Flags](https://docs.microsoft.com/windows/win32/procthread/process-creation-flags)
- [Windows App SDK deployment guide for unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps)
