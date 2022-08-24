---
page_type: sample
languages:
- cpp
products:
- windows
- windows-api-win32
- windows-app-sdk
- DXGI
name: DynamicRefreshRateTool sample
urlFragment: DynamicRefreshRateTool-sample
description: Demonstrates how to use APIs related to the Dynamic Refresh Rate feature with WinUI3 applications.
extendedZipContent:
- path: LICENSE
- target: LICENSE
---

# DynamicRefreshRateTool sample

This sample application demonstrates how a **WinUI3** app can interact with the Windows 11 feature [Dynamic Refresh Rate](https://devblogs.microsoft.com/directx/dynamic-refresh-rate/) through various system APIs. It uses these APIs to both demonstrate their functionality and provide a helpful and easy-to-use framework for validating the functionality of Dynamic Refresh Rate on supported devices, both programatically and visually. The main purpose will be to illustrate when the refresh rate of a system changes by going higher and lower. We refer to this functionality as *boosting* and *unboosting.* You can read more about how this feature works in our documentation for the [Compositor Clock API](https://docs.microsoft.com/en-us/windows/win32/directcomp/compositor-clock/compositor-clock'). 

## APIs Used

**WinUI3** controls are used to build the main user interface of this application, including its windows and buttons. For the animation shown in the animations tab, we use **Microsoft.UI.Composition**. In this iteration of **Microsoft.UI.Composition**, the app's visual scene is self-composed into a single **DXGI SwapChain**. Due to the design of Dynamic Refresh Rate, DXGI SwapChains are mapped to a [virtualized refresh rate](https://docs.microsoft.com/en-us/windows/win32/directcomp/compositor-clock/compositor-clock) by default and are not able to perceive changing vblank cadences of present timings. In build 22502, Windows 11 introduced [DXGIDisableVBlankVirtualization](https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_6/nf-dxgi1_6-dxgidisablevblankvirtualization) which DynamicRefreshRateTool uses to disable the virtualization for the application before the main SwapChain is created, allowing the SwapChain to observe changes in its [IDXGIOutput::WaitForVBlank](https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgioutput-waitforvblank) and present timing as the refresh rate changes from 60Hz to 120Hz and back. We expect many applications that are SwapChain-based will use this method to be able to perceive the Dynamic Refresh Rate.

The buttons that are used to request boost use the [DCompositionBoostCompositorClock](https://docs.microsoft.com/en-us/windows/win32/api/dcomp/nf-dcomp-dcompositionboostcompositorclock) function with a boolean value to indicate whether the application is requesting the system boost or rescinding the request, which we call boosting or unboosting. 

The Statistics tab showcases usage of [Frame statistics](https://docs.microsoft.com/en-us/windows/win32/api/dcomp/nf-dcomp-dcompositiongetstatistics) through DComposition-based statistics API.  

## Dynamic Refresh Rate behavior on different Windows 11 versions

While using this sample, you may find that boosting may occur at different times when your application does not request it. That is because there is global logic in Windows that will *auto-boost*. In Windows 11 builds before [preview 22557](https://blogs.windows.com/windows-insider/2022/02/16/announcing-windows-11-insider-preview-build-22557/), auto-boosting occurs when GPU Accelerated Ink is drawn, such as by using [DelegatedInkTrailVisual ](https://docs.microsoft.com/en-us/uwp/api/windows.ui.composition.delegatedinktrailvisual?view=winrt-22621) as well as when an application uses [InteractionTracker](https://docs.microsoft.com/en-us/uwp/api/windows.ui.composition.interactions.interactiontracker?view=winrt-22621) to drive animations. After 22557 [Windows will auto-boost for a short duration when it receives pointing input](https://blogs.windows.com/windows-insider/2022/02/16/announcing-windows-11-insider-preview-build-22557/#:~:text=We%E2%80%99re%20expanding%20Dynamic,then%20restart%20Edge.).

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).


## Building and running the sample

* Open the solution file (`.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

In the project properties, select the "Debug" tab, and set the Debugger type for both "Application process" and "Background
task process" to "Native Only".

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Compositor Clock](https://docs.microsoft.com/en-us/windows/win32/directcomp/compositor-clock/compositor-clock)
- [Dynamic Refresh Rate](https://devblogs.microsoft.com/directx/dynamic-refresh-rate/)
- [DXGIDisableVBlankVirtualization](https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_6/nf-dxgi1_6-dxgidisablevblankvirtualization)
- [Windows 11 preview 22557](https://blogs.windows.com/windows-insider/2022/02/16/announcing-windows-11-insider-preview-build-22557/)
