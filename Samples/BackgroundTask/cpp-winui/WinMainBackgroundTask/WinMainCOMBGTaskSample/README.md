---
page_type: sample
languages:
- cpp
products:
- windows
- windows-api-win32
- windows-app-sdk
name: WinMainCOMBGTaskSample sample
urlFragment: WinMainCOMBGTaskSample
description: Demonstrates how to use register background task in a win32 application.
extendedZipContent:
- path: LICENSE
- target: LICENSE
---

# WinMainCOMBGTaskSample sample

This sample application demonstrates how a **WinUI3** app can register Background Task. In this sample usage of ToastNotificationHistoryChangedTrigger is demonstrated which can be easily changed to any supported triggers. On clicking the button on the UI, background task gets registered and a toast notification will get pushed, when user deletes this toast notification background task gets triggered and push another toast notification to notify that BackgroundTask have been triggered.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).


## Building and running the sample

* Open the solution file (`.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
