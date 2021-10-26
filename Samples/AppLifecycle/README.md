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

Non-UWP apps can also be packaged into MSIX packages as Desktop Bridge apps. While these apps can use some of the Windows App SDK AppLifecycle features, they must use the manifest approach where this is available. For example, they cannot use the Windows App SDK RegisterForXXXActivation APIs and must instead register for rich activation via the manifest.

All the constraints for packaged apps also apply to WinUI apps (because they are packaged apps) - and there are additional considerations specific to WinUI apps as described below.

### Rich activation

#### GetActivatedEventArgs

- Unpackaged apps: Fully usable.
- Packaged apps: Usable, but these apps can also use the platform GetActivatedEventArgs. Note: the platform defines Windows.ApplicationModel.AppInstance whereas the Windows App SDK defines Microsoft.Windows.AppLifecycle.AppInstance, which are similar but different. Note also that while UWP apps can use the *ActivatedEventArgs classes (eg, FileActivatedEventArgs, LaunchActivatedEventArgs, and so on), apps that use the Windows App SDK AppLifecycle feature must use the interfaces not the classes (eg, IFileActivatedEventArgs, ILaunchActivatedEventArgs, and so on).
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
- STAs and async operations: By default, certain apps specify that their Main/WinMain function runs in a Single-Threaded Apartment (STA). The RedirectActivationToAsync API is an async method, and the caller must wait for the method to complete  before exiting. This method should be called as early as possible in the life of the app before it performs any work - such as creating windows or any other initialization - that would be redundant if the app is redirecting its activation. However, waiting for completion in Main/WinMain is problematic because this would block the STA. This applies to Windows Forms, WPF and WinUI apps. For Windows Forms and C# WinUI, you can solve the problem by declaring Main/WinMain to be async, and then using the non-blocking await mechanism on the call to RedirectActivationToAsync. For WPF apps, you cannot declare Main to be async because this causes problems in creating windows. Similarly, C++ WinUI apps cannot use the async/await mechanisms which are only available in C#. For WPF and C++ WinUI apps, one option is to move the RedirectActivationToAsync call onto another thread, use an event to signal when it is completed, and use a non-blocking wait API such as CoWaitForMultipleObjects or MsgWaitForMultipleObjects to wait for this signal. See the sample code for examples of how to do this. NOTE: this added complexity for certain app types leads us to consider refining the API in a later release.

### Power/State notifications

- Unpackaged apps: Fully usable.
- Packaged apps: Fully usable.

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)

- [Rich activation](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-rich-activation)

- [App Instancing](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-instancing)

- [Power/state notifications](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/applifecycle/applifecycle-power)

  

  
