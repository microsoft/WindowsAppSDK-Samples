---
name: "App Notifications Sample" 
page_type: sample
languages:
- cpp
products: 
- windows-app-sdk
description: "Demonstrates how to use the windows app SDK App Notifications APIs from an unpackaged WinUI app"
urlFragment: Notifications/App
---
# App Notifications Sample
This sample demonstrates the following scenarios:

**Scenario 1: Local Toast with Avatar Image**

Send a toast with an embeded image with a button that allow the user to activate the sample, whether it is still running or not.

**Scenario 2: Local Toast with Avatar and Text Box**

Send a toast with a textbox that let the user type some text and, when reply is pressed, activate the sample, whether it is still running or not and retrieve the text from the notification.

## Building and running the App Notiffication samples 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

# Adding a New Scenario to the Sample
All samples in this directory should be kept in-sync, meaning that if you add a scenario to one (say the C++ unpackaged sample), you should also add it ti all the others.
The samples in this directory are differenciated by the language the use C++ vs C# or the type of packaging but not by their functionality.

1. **Implement the UI** -
Each secenario is implemented in its own distinct set of files. The UI is Modify the xaml, UI implementation and notification implementation files as needed, adding removing and changing controls to suit your needs.
   - **For a C++ sample** -
   That means adding three 3 new files to the VS project: <NewScenario>.xaml, <NewScenario>.xaml.cpp and <NewScenario>.xaml.h then adding an entry for the new scenario in the Project.idl and the SampleConfiguration.cpp files.
   - **For a C# sample** -
   That means adding three 2 new files to the VS project: <NewScenario>.xaml> and <NewScenario>.xaml.cs> then adding an entry for the new scenario in the Project.idl and the SampleConfiguration.cs files.

2. **Implement the Notification files** -
In the App Notification samples, the Notification Implementation code is kept separated from the UI code to make it easier for devs to retrieve the bits of code they need for their own WindowsAppSDK app.
   - **For the C++ sample** -
   That means creating 2 files: a <NewScenario>.cpp and a <NewScenario>.h files.

   - **For the C# sample** -
   That means creating 1 files: a <NewScenario>.cs file.

3. **Set up your notification handler** -
Your new implementation should have one function (sometimes more) to handle activation from the toasts posted by your scenario.
That function should be added to the c_NotificationHandlers map in NotificationManager.cpp (.cs), otherwise user interaction with a toast won't be properly routed back to your scenario.

4. **Update the documentation** -
Update the documentation for the sample (this document) with the information pertinent to your new scenario.
At the very least, that means adding an entry to describe your scenario, near the top of this document.

5. **Open a PR.** -
Verify your new scenario builds (without warnings), that it works as expected and has been implemented for all flavors of the sample (C++/C#, Packaged/Unpackaged, etc.).
Go to the [WindowsAppSDK-Samples](https://github.com/microsoft/WindowsAppSDK-Samples) GitHub and follow the Contributing Guidelines (at the bottom of the readme) and submit ytour code changes for peer review.

## Notes
Scenario do not persist. When the user move between scenario, the old scenario is deleted and a new one is create.
Keep this in mind when designing your own scenario.

Error, warning and information messages that are displayed at the bottom of the sample window are kept in NotifyUser.cpp (.cs).
Your scenario can reuse any message arlready defined or defines its own new message if needed.
Try keeping all messages together for easy reference. 

Before implementing your scenario for all flavors of the sample, it's a good idea to open adraft PR for your sample and gather ealy feedback.

---
**Why is the notification implementation in separate files?**

*The examples for calling the Notification APIs are located in a common directory away from the sample scenario files, why is that?*
*Wouldn't it have been more natural to have the calls to the Notification APIs directly inside the sample scenario files?*
