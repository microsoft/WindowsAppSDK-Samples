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

# Contributing to this project
- Refer to the [contributing guide](./Contributing.md)

## Special Considerations for Unpackaged & Self-Contained Mode with WCR

- This project is not intended to be fully functional when running as an unpackaged app. However, Windows Copilot Runtime (WCR) does support unpackaged apps.
- Self-contained mode is fully supported by WCR APIs as well.
- The following command demonstrates how to run the app as an ARM64 unpackaged application in self-contained mode (for both WinAppSDK and .NET):
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsPackageType=None -p:WindowsAppSDKSelfContained=true -p:SelfContained=true
```
- One careful consideration while using self-contained mode. The OS ACL permissions prevent it to run inside any folder in `C:\Users` like `Downloads` because `WorksloadsSessionManager` running as a local service, cannot load WCR dlls from that folder with default permissions. This is by security choice, by design. The two ways to solve it are a) Move the self-contained folder out of `C:\Users` where ACLs are not too restrictive or b) provide Local Service to access the said self-contained folder within `C:\Users`. Only affects self-contained mode, it doesn't affect other config modes.
