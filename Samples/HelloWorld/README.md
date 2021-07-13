# Windows App SDK Hello World Sample

This C++ sample demonstrates an instance of the "Blank App, Packaged (WinUI in Desktop)" project template (available in WinUI 3) that has been configured to include the Project Reunion NuGet package. This sample simply displays a button that, when pressed, makes a call to AppLifecycle::GetActivatedEventArgs() to show that Windows App SDK APIs load and run correctly. You can use this sample as a starting point for exploring Windows App SDK APIs. For more info about the available APIs, see the following links.

* [MRT Core](https://docs.microsoft.com/windows/apps/windows-app-sdk/mrtcore/mrtcore-overview)
* [DWrite Core](https://docs.microsoft.com/windows/apps/windows-app-sdk/dwritecore)

## How to run the sample

1. Follow steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk#set-up-your-development-environment) to set up environment.

2. Clone this repository and open ReunionCppDesktopSampleApp.sln in Visual Studio 2019 Preview

3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

    > Note: When building the solution, if you get an error related to a PowerShell command: </br></br>`The command "powershell.exe -ExecutionPolicy Unrestricted -file ..." exited with code 1.` </br></br> You will need to manually run that PowerShell command to build. Copy the full PowerShell command inside the quotes of the error message and paste it into a command prompt.

4. To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.

## System requirements
 * Windows 10 Version 1809 or higher

## Related links
- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
