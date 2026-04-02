---
page_type: sample
languages:
- cppwinrt
- cpp
products:
- windows
- windows-app-sdk
name: "Simple Island App"
urlFragment: SimpleIslandApp
description: "Shows how to add a WinAppSDK island to a simple Win32 app."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Simple Island App

This sample shows how to add a WinAppSDK island with Xaml content to a Win32 app.  It was first created with the C++ "Windows Desktop Application"
template in Visual Studio, which yields a boilerplate Win32 app that uses Windows APIs that have been around for a long time.

This sample is an "unpackaged" app, so it will run like a win32 app that hasn't been changed for a while.

This sample uses Windows App SDK as a "framework package".  This means that the Windows App SDK runtime must be installed for it to run.

## What is a WinAppSDK Island?

A WinAppSDK island is a set of APIs that allows an app author to connect two UI frameworks together.  This sample demonstrates how
to connect Xaml content into a Win32 app.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running the sample

* Open the solution file (`.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* Press Ctrl+F5 to launch the app (without attaching a debugger)
    > Note: If the Windows App SDK runtime isn't installed on the machine, the user will see a message box directing them to a download link.
* Press F5 to launch the app under a debugger.
    > Note: When running under a debugger, you may see an "Exception Thrown" dialog box in Visual Studio.  You can safely press the "Continue"
    button to proceed.
* To run from the command line or File Explorer, navigate to `<arch>/<config>/SimpleIslandApp` directory and run SimpleIslandApp.exe.
* To deploy to another machine, copy the `<arch>/<config>/SimpleIslandApp` directory to that machine and run SimpleIslandApp.exe.  The sample
runs on Windows version 17763 and later.

# How to add an Island with Xaml content to your own Win32 app

Here's the basic steps to add an island to your own Win32 app, leveraging code from this sample:
1. Add a NuGet reference to the Microsoft.WindowsAppSDK package (latest stable version).
2. Add a NuGet reference to the Microsoft.Windows.CppWinRT package (latest stable version).
3. Make some modifications to your vcxproj file and main message loop -- look for the tag "Island-support" in the sample code.
4. Add an implementation for your Xaml App object.  Feel free to copy the App.* files from this sample, as well as the vcxproj entries.
The App object is needed for many of the Xaml controls to work, and it also enables metadata lookups for your app.
5. Optional: Add a MainPage to create your first page of Xaml.  You can copy it from this sample app.
6. Modify your application to create your Xaml App object.  After that, create a DesktopWindowXamlSource object to hold
your Xaml content and position it within your HWND in your app wherever you'd like.

