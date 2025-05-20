---
page_type: sample
languages:
    - csharp
products:
    - windows
    - windows-app-sdk
name: "Windows AI Samples"
urlFragment: WindowsAISamples
description: "Shows how to use the Windows AI apis"
extendedZipContent:
    - path: LICENSE
      target: LICENSE
---

# Windows AI Samples

An app that demonstrates how to use the Windows AI apis with WinUI.

## Releases
Stable features can be found in the [main](https://github.com/microsoft/WindowsAppSDK-Samples/tree/main/Samples/WindowsAIFoundry/cs-winui) branch. 

Experimental features can be found in the [release/experimental](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/experimental/Samples/WindowsAIFoundry/cs-winui) branch

## Prerequisites

-   See
    [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
-   Make sure that your development environment is set up correctly&mdash;see
    [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
-   Use a Copilot+ PC
-   Detailed instructions : [Get started building an app with Windows AI APIs](https://learn.microsoft.com/en-us/windows/ai/apis/model-setup)


To get the latest updates to Windows and the development tools, and to help shape their development,
join the [Windows Insider Program](https://insider.windows.com).

## Building and running the sample

-   Open the solution file (`.sln`) in Visual Studio.
-   Ensure your build configuration is set to `arm64`.
-   From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

# Contributing to this project
- Refer to the [contributing guide](./Contributing.md)

## Special Considerations for Unpackaged and Self-Contained modes with WinAI Apis

- Unpackaged app configuration is no longer supported. Every app using WinAI APIs must have an identity. One can use [sparse package config](https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps) to achieve that with unpackaged binaries.
- Self-contained mode is fully supported by Windows AI apis as well.
- The following command demonstrates how to run the app as an ARM64 application in packaged self-contained mode (for both WinAppSDK and .NET):
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsAppSDKSelfContained=true -p:SelfContained=true.
```