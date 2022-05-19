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

## Building and running the sample 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

# Adding a New Scenario to the Sample
All samples in this directory should be kept in-sync, meaning that if you add a scenario to one,you should also add it the to others.
The samples in this directory are differenciated by the language the use C++ vs C# or the type of packaging but not by the functionality.

1. Create a new scenario (by duplicating and renaming an existing one)
   - For a C++ sample, that means duplicating 3 files:
   - For a C# sample that means duplicating 2 files: 
2. Add the new scenario to the ScenarioManager class.
3. At this point, the sample should build and the new scenario should be available, albeit an exact copy of another scenario.
4. Duplicate the Notification implementation files located in the directory
5. Modify the xmal, UI implementation and notification implementation files as needed, adding removing and changing controls to suit your needs.
6. Update the documentation for the sample (this document) with the information pertinent to your scenario
7. Open a PR.

---
**Why is the notification implementation in separate files?**

*The examples for calling the Notification APIs are located in a common directory away from the sample scenario files, why is that?*
*Wouldn't it have been more natural to have the calls to the Notification APIs directly inside the sample scenario files?*



---
**?**

**



---
**?**

**
