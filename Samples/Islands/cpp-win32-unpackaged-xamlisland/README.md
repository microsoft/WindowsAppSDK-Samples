# Xaml Island App

This samples takes the Simple Island App sample and refactors it to work with a `XamlIsland` instead of a `DesktopWindowXamlSource`.
The `XamlIsland` works similarly to the `DesktopWindowXamlSource` in that it allows for fragments of Xaml content
to be hosted inside an application. However, it does not create its own `DesktopChildSiteBridge`. Instead,
it exposes a `ContentIsland`, allowing the developer to host the island inside a bridge that the app creates.

This sample is an "unpackaged" app, so it will run like a win32 app that hasn't been changed for a
while. This sample uses Windows App SDK as a "framework package".  This means that the Windows App
SDK runtime must be installed for it to run.


## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* Since the XamlIsland type is yet to be released, this app runs with an updated version of the WindowsAppSDK taken from this [pipeline run](https://dev.azure.com/microsoft/ProjectReunion/_build/results?buildId=83011140&view=results).
Place the updated nupkg into the local packages directory and install the runtime from the build's artifacts.
