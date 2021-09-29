---
page_type: sample
languages:
- cpp
- cs
products:
- windows
- windows-api-win32
- windows-app-sdk
name: AppLifecycle samples
urlFragment: AppLifecycle
description: Demonstrates the AppLifecycle APIs, including Activation, Instancing, State Notifications and Environment Variables.
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# AppLifecycle samples

These sample applications demonstrate the AppLifecycle APIs, which cover the following feature areas:

- Rich activation and activation arguments
- Selective single-/multi-instancing
- Power and system state notifications.

## System requirements

* Windows 10

## Building and running the samples

Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment). For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio.
2. Build the solution.
3. Press F5 to deploy and debug the project.

## Notes for different app types

Most of the AppLifecycle features are features that already exist in the UWP platform, and have been brought forward to the Windows App SDK so that all app types can use them - and in particular, unpackaged app types such as Console apps, Win32 apps, Windows Forms apps, and WPF apps. These features cannot be used in UWP apps - and there is no need for a UWP app to use the Windows App SDK implementations, since there are equivalent features in the UWP platform itself.

Non-UWP apps can also be packaged into MSIX packages, with an MSIX app manifest - and these are known as Desktop Bridge apps. While these apps can use some of the Windows App SDK AppLifecycle features, they must use the manifest approach where this is available. For example, they cannot use the Windows App SDK RegisterForXXXActivation APIs and must instead register for rich activation via the manifest.

All the constraints for packaged apps also apply to WinUI apps (because they are packaged apps) - and there are additional considerations specific to WinUI apps as described below.

### Rich activation

#### GetActivatedEventArgs

- Unpackaged apps: Fully usable.
- Packaged apps: Usable, but these apps can also use the platform GetActivatedEventArgs. Note: the platform defines Windows.ApplicationModel.AppInstance whereas the Windows App SDK defines Microsoft.Windows.AppLifecycle.AppInstance, which are similar but different.
- WinUi apps: WinUI's App.OnLaunched is given a Microsoft.UI.Xaml.LaunchActivatedEventArgs, whereas the platform GetActivatedEventArgs returns a Windows.ApplicationModel.IActivatedEventArgs, and the WindowsAppSDK GetActivatedEventArgs returns a Microsoft.Windows.AppLifecycle.AppActivationArguments object which can represent a platform LaunchActivatedEventArgs.

#### RegisterForXXXActivation

- Unpackaged apps: Fully usable.
- Packaged apps: Not usable - use the app's MSIX manifest instead.

#### UnregisterForXXXActivation

- Unpackaged apps: Fully usable.
- Packaged apps: Not usable - manifested registrations are removed when the app is uninstalled.

### Single/Multi-instancing

- Unpackaged apps: Fully usable.
- Packaged apps: Fully usable.
- WinUI apps: If an app wants to detect other instances and redirect an activation, it must do so as early as possible, and before initializing any windows, etc. To enable this, the app must define DISABLE_XAML_GENERATED_MAIN, and write a custom Main (C#) or WinMain (C++) where it can do the detection and redirection.

### Power/State notifications

- Unpackaged apps: OK.
- Packaged apps: OK.

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)

- [Rich activation](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-rich-activation)

- [App Instancing](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-instancing)

- [Power/state notifications](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-power)

  

  
