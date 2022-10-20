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

This project includes a sample of a 3rd party widget provider. At this point, it's only possible to author widgets in C++ and Widgetprovider application must be a packaged application.

## Converting sample from Console to Windows application

If the widget providing application doesn't have any UI - it's a good idea to convert the app to windows application so the console doesn't show to the end user.
These are the required steps to convert the sample to windows application:
- Right click on the SampleWidgetProviderApp in Visual Studio and open Properties. Navigate to Linker -> System and change SubSystem from Console to Windows. It can also be done by adding `<SubSystem>Console</SubSystem>` to the `<Link>..</Link>` section of the .vcxproj.
- Substitute `int main()` in `main.cpp` with `int WINAPI wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ PWSTR pCmdLine, _In_ int /*nCmdShow*/)`.

## Debugging WidgetProvider

After the solution have been built, you will need to deploy the `SampleWidgetProviderAppPackage`. In order to do that, right click on the `SampleWidgetProviderAppPackage` and click Deploy. Follow all the required prompts and finish deploying.
Once you've done that, you may navigate to the Widgets Dashboard (hower over/click the widgets icon in the left bottom corner of the taskbar or use the shortcut `WIN + W`) and pin your widget.
In order to do that:
- Open Widgets Dashboard.
- Click `+` sign next to your user avatar.
- Find your widget and click `+` sign next to it.
- Observe it on the widgets board.

After you have pinned your widgets, the Widget Platform will start your widget provider application in order to receive and send relevant information about the widget. 
To debug the running widget you can either attach to a debugger to the running widget provider application or you can set the provider to automatically attach to the widget provider process once it spins up.

In order to attach to the running process:
- In Visual Studio click Debug -> Attach to process.
- Filter the processes and find your desired widget provider application.
- Attach the debugger.

In order to automatically attach the debugger to the process when it's initially started:
- In Visual Studio click Debug -> Other Debug Targets -> Debug Installed App Package
- Filter the packages and find your desired widget provider package.
- Select it and check the box that says `Do not launch, but debug my code when it sarts`.
- Click Attach. 

## WidgetProvider app distribution

The widget provider app can be distributed in 2 ways:
- With the Developer Mode turned on the machine the packaged widget provider application can be sideloaded.
- Otherwise, it can be distributed through the Microsoft Store.

Relevant documentation and full walkthrough of a similar sample may be found [here](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32).

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Learn more about Windows Widgets](https://learn.microsoft.com/windows/apps/design/widgets/)
- [Implement a Win32 widget provider](https://learn.microsoft.com/windows/apps/develop/widgets/implement-widget-provider-win32)
