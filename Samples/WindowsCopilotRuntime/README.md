---
page_type: sample
languages:
    - csharp
products:
    - windows
    - windows-app-sdk
name: "Windows Copilot Runtime"
urlFragment: WindowsCopilotRuntime
description: "Shows how to use the Windows Copilot Runtime"
extendedZipContent:
    - path: LICENSE
      target: LICENSE
---

# Windows Copilot Runtime Sample

An app that demonstrates how to use the Windows Copilot Runtime with WinUI.

## Prerequisites

-   See
    [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
-   Make sure that your development environment is set up correctly&mdash;see
    [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
-   Use a Copilot+ PC
-   Detailed instructions : [Set up your development environment to build Windows Copilot Runtime APIs](https://learn.microsoft.com/en-us/windows/ai/apis/model-setup)


To get the latest updates to Windows and the development tools, and to help shape their development,
join the [Windows Insider Program](https://insider.windows.com).

## Building and running the sample

-   Open the solution file (`.sln`) in Visual Studio.
-   Ensure your build configuration is set to `arm64`.
-   From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## Special Considerations for Debugging as an Unpackaged App in Visual Studio

- This project is not designed to be fully functional in an unpackaged app. However, Windows Copilot Runtime does support unpackaged app.
- To enable proper startup as an unpackaged app, you need to bootstrap the Windows App SDK either Programmatically or By adding the following configuration to the `.csproj` file during the build process:
 ```xml
  <WindowsAppSdkBootstrapInitialize>true</WindowsAppSdkBootstrapInitialize>
 ```
- Alternatively, if you are using the Developer Command Prompt for Visual Studio, you can run the app as an ARM64 version using the following command, with the bootstrap property provided:
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsPackageType=None -p:WindowsAppSdkBootstrapInitialize=true
```
