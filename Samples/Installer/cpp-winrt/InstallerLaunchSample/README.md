---
name: "Installer launch sample" 
page_type: sample
languages:
- cpp
products:
- windows
- windows-api-win32
description: "Shows how to use CreateProcess to launch the installer without a console window."
urlFragment: installer
---
# Installer launch sample

This sample demonstrates how to launch the WindowsAppRuntimeInstall.exe without having a pop-up
window appearing while it is running. This sample is intended for Win32 programs which have setup
programs that need to embed the WindowsAppSDK without any pop-up windows.

## Requirements
Sample requires downloading or building a version of WindowsAppRuntimeInstall.exe. The sample
assumes that it will be in %USERPROFILE%\Downloads for convenience. If a different path is
desired, it can be specified in the #define at the top in main.cpp.

## Building and running any of the samples 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

1. Download or build a version of the WindowsAppRuntimeInstall.exe, and place it in
%USERPROFILE%\Downloads.
2. Open the solution file (.sln) in the sample.
3. Build the solution.
4. Debug the project.

*Caution, this sample will execute whatever executable is at the specified path. It is recommended
to build the Installer with test packages from the WindowsAppSDK project or use a simple "Hello,
World" console application as the executable to avoid modifying the system with the sample.*

## Related Links

- [Creating Processes](https://docs.microsoft.com/windows/win32/procthread/creating-processes)
- [Process Creation Flags](https://docs.microsoft.com/windows/win32/procthread/process-creation-flags)
