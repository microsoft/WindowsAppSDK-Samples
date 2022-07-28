---
page_type: sample
languages:
- cpp
- csharp
products:
- windows
- windows-app-sdk
name: "C# Windows Runtime Component WinUI Controls Sample"
urlFragment: csruntimecomponent
description: "Shows how to author a Windows Runtime Component in C# with WinUI controls and how to consume it from C++ and C#."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Custom Controls Sample (C# Windows Runtime Component)

This sample demonstrates how to author C# components with WinUI controls (User Control and Custom Control) using C#/WinRT, and how to consume these components from C++ and C# apps.

The sample solution includes the following application projects: 

- **CppApp** (packaged C++ WinUI app)
- **CppAppUnpackaged** (unpackaged C++ WinUI app)
- **CsApp** (packaged C# WinUI app)

All the application projects have a project reference to the C# Windows Runtime component library project, **WinUICsComponent**. The C# component uses the *Microsoft.Windows.CsWinRT* NuGet package to generate a Windows Runtime Component with custom WinUI controls.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running the sample

* Open the solution file (`.sln`) in Visual Studio.
* Set one of the application projects as the startup project.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Related Links

- [C#/WinRT Authoring](https://github.com/microsoft/CsWinRT/blob/master/docs/authoring.md)
- [Walkthrough—Create a C# component with WinUI 3 controls, and consume it from a C++/WinRT app that uses the Windows App SDK](https://docs.microsoft.com/windows/apps/develop/platform/csharp-winrt/create-winrt-component-winui-cswinrt)
