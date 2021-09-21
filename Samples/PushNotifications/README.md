---
name: "Push Notifications Sample" 
page_type: sample
languages:
- cpp
products: 
- windows-app-sdk
description: "Shows how to use the windows app SDK Push Notifications APIs from an unpackaged app"
urlFragment: PushNotifications
---
# Push Notifications Sample

These samples demonstrate how to use the AppWindow class to manage your application's window.  

## Requirements
Cpp sample requires a ... like Postman or fiddler to send notifications.
* The WindowsAppSDK push notifications APIs defines all that is needed to register for and receive notifications but it's the role of an external app to send the notifications.
This ... (postman, fiddler) is used to eimulate that external server and sends notifications.

## Building and running any of the samples 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio 2019 Preview.
2. Build the solution.
3. Right click on the solution and click **Deploy Solution**
4. Debug the project.

## Related Links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Push Notifications Doc]()
