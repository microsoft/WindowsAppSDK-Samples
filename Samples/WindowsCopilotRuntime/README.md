---
page_type: sample
languages:
    - csharp
    - cpp
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

An app that demonstrates how to use the Windows Copilot Runtime with WinUI, WPF, C#, C++, and other build
environments.

## Prerequisites

-   See
    [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
-   Make sure that your development environment is set up correctly&mdash;see
    [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
-   Use a Copilot+ PC

To get the latest updates to Windows and the development tools, and to help shape their development,
join the [Windows Insider Program](https://insider.windows.com).

## Building and running the sample

-   Open the solution file (`.sln`) in Visual Studio.
-   From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

See [additional instructions](./cpp-console-unpackaged/README.md) for using [the C++ & CMake sample](./cpp-console-unpackaged/CMakeLists.txt).

## Special Considerations for Debugging as an Unpackaged App in Visual Studio

- This project is not designed to be fully functional in an unpackaged app. However, Windows Copilot Runtime does support unpackaged app.
- To enable proper startup as an unpackaged app, you need to bootstrap the Windows App SDK either Programmatically or By adding the following configuration to the `.csproj` file during the build process:
 ```xml
  <WindowsAppSdkBootstrapInitialize>true</WindowsAppSdkBootstrapInitialize>
  <WindowsPackageType>None</WindowsPackageType>
 ```
- Alternatively, if you are using the Developer Command Prompt for Visual Studio, you can run the app as an ARM64 version using the following command, with the bootstrap property provided:
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsPackageType=None -p:WindowsAppSdkBootstrapInitialize=true
```
- Self contained mode is fully supported by WCR apis.     
- One careful consideration while using self contained mode. The OS ACL permissions prevent it to run inside any folder in `C:\Users` like `Downloads` because `WorksloadsSessionManager` running as a local service, cannot load WCR dlls from that folder with default permissions. This is by security choice, by design. The two ways to solve it are a) Move the self contained folder out of `C:\Users` where ACLs are not too restrictive or b) provide Local Service to access the said self contained folder within `C:\Users`. Only affects self contained mode, it doesn't affect other config modes.
