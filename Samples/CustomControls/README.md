---
page_type: sample
languages:
- cpp
- csharp
products:
- windows
- windows-app-sdk
name: "WinUI Custom Controls C# Authoring Sample"
urlFragment: cswinrtauthoring
description: "Shows how to author WinUI controls with C#/WinRT and consume them from C++ and C#."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Custom Controls Sample (C#/WinRT Authoring)

This sample demonstrates how to author C# components with WinUI controls (User Control and Custom Control) using C#/WinRT, and how to consume these components from C++ and C# apps.

The sample solution includes two application projects: **CppApp** (packaged C++ WinUI app) and **CsApp** (packaged C# WinUI app). Both of the application projects have a project reference to the C# component, **WinUICsComponent**. The C# component uses the *Microsoft.Windows.CsWinRT* NuGet package to generate a Windows Runtime Component with custom WinUI controls.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running the sample

* Open the solution file (`.sln`) in Visual Studio.
* Set either the **CppApp** or **CsApp** project as the startup project.
* Select `x86` as the Solution platform. There is a known issue with `x64` and `arm64` (https://github.com/microsoft/CsWinRT/issues/1093).
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [C#/WinRT Authoring](https://github.com/microsoft/CsWinRT/blob/master/docs/authoring.md)
