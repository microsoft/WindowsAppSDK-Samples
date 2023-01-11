---
page_type: sample
languages:
- cpp
- cppwinrt
products:
- windows
- windows-app-sdk
name: "Windows Widgets Samples"
urlFragment: Widgets
description: "Shows how to author Windows Widgets with WindowsAppSDK"
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Windows Widgets samples

These samples demonstrate how to author Windows Widgets with the Windows App SDK.

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
    * For the unpackaged sample apps, make sure to also install the prerequisites for [deploying unpackaged apps](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

## Building and running any of the samples

* Open the solution file (`.sln`) from the subfolder of your preferred sample in Visual Studio.
* From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

## WidgetProvider sample

This project includes a sample of a 3rd party widget provider. The sample is a packaged application that is authored in both C++ and C#.
## Converting sample from Console to Windows application

If the widget providing application doesn't have any UI - it's a good idea to convert the app to windows application so the console doesn't show to the end user.
These are the required steps to convert the sample to windows application:
- For C++
  - Right click on the SampleWidgetProviderApp in Visual Studio and open Properties. Navigate to Linker -> System and change SubSystem from Console to Windows. It can also be done by adding `<SubSystem>Windows</SubSystem>` to the `<Link>..</Link>` section of the .vcxproj.
  - Substitute `int main()` in `main.cpp` with `int WINAPI wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ PWSTR pCmdLine, _In_ int /*nCmdShow*/)`.
- For C#
  - Right click on CsConsoleWidgetProvider in Visual Studio and open Properties. In the Output type dropdown choose `Windows Application`.

## Debugging WidgetProvider

After the solution has been built, you will need to deploy the `SampleWidgetProviderAppPackage(C++)` or `CsConsoleWidgetProvider(C#)`. In order to do that, right click on the desired project and click Deploy. 
Once you've done that, you may navigate to the Widgets Dashboard and pin your widget:
- Open Widgets Dashboard by hovering over or clicking the widgets icon in the left bottom corner of the taskbar, or by using the shortcut `WIN + W`.
- Click the `+` sign next to your user avatar.
- Find your widget and click the `+` sign next to it.
- Observe it on the widgets board.

After you have pinned your widgets, the Widget Platform will start your widget provider application in order to receive and send relevant information about the widget. 
To debug the running widget you can either attach a debugger to the running widget provider application or you can set up Visual Studio to automatically start debugging the widget provider process once it's started.

In order to attach to the running process:
- In Visual Studio click Debug -> Attach to process.
- Filter the processes and find your desired widget provider application.
- Attach the debugger.

In order to automatically attach the debugger to the process when it's initially started:
- In Visual Studio click Debug -> Other Debug Targets -> Debug Installed App Package.
- Filter the packages and find your desired widget provider package.
- Select it and check the box that says `Do not launch, but debug my code when it sarts`.
- Click Attach. 

## WidgetProvider app distribution

The widget provider app can be distributed in 2 ways:
- Sideloading - this requires Developer Mode to be turned on.
- Through the Microsoft Store.

For more information see [Distribute your packaged desktop app](https://learn.microsoft.com/windows/apps/desktop/modernize/desktop-to-uwp-distribute).

Relevant documentation and full walkthrough of a similar sample may be found [here](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32).

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Learn more about Windows Widgets](https://learn.microsoft.com/windows/apps/design/widgets/)
- [Implement a Win32 widget provider](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32)
