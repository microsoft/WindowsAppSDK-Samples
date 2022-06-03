---
name: "Push Notifications Sample" 
page_type: sample
languages:
- cpp
products: 
- windows-app-sdk
description: "Demonstrates how to use the windows app SDK Push Notifications APIs from an unpackaged app"
urlFragment: PushNotifications
---
# Push Notifications Sample
This sample demonstrates:
1. How an app can use the Windows App SDK to request a WNS Channel URI
    * The WNS Channel URI uniquely identifies the app for a current user on a specific device, allowing the app server to send notifications to the app.
2.	How to use the Windows App SDK to register an app for foreground and background activation and handle the activations.
## Requirements
* A web API tool (like Postman or fiddler) to send notifications.
   * The Windows App SDK push notifications APIs provides an API to register for and receive notifications from an external app service. The web API tool is used to emulate the external app service.
* An Azure AppId for your app.
   * To obtain an Azure AppId, follow "Configure your app's identity in Azure Active Directory" [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push-notifications/push-quickstart#configure-your-apps-identity-in-azure-active-directory-aad).
* Windows 10
## Building and running the sample 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).
### To build the sample
1. Open the solution file (.sln) in the subfolder of the sample in Visual Studio.
2. Replace the guid in the following line (near the top of the sample) with your own Azure AppId:
``` cpp
winrt::guid remoteId{ "00000000-0000-0000-0000-000000000000"}; 
```
3. Build the solution. Press **Ctrl+Shift+B**, or select **Build** \> **Build Solution**.
4. Debug the project. Press **F5**, or select **Debug** \> **Start Debugging**. 

The sample will display a WNS Channel URI which uniquely identify this app, for this user on this device and thus can be used by other apps (usually a server app) to send notifications to this specific app.
## Receiving Notifications
Follow these [instructions](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push-notifications/push-quickstart#send-a-push-notification-to-your-app) to send a notification to the sample. 
   * Leave the sample running to receive notifications in the foreground.
   * Close the sample to receive notifications in the background.
## Related Links
- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Push notifications overview](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push-notifications/)
- [Quickstart Push notifications in the Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push-notifications/push-quickstart)
- [Microsoft.Windows.PushNotifications Namespace](https://docs.microsoft.com/windows/windows-app-sdk/api/winrt/microsoft.windows.pushnotifications)
## Common Issues
* If there was an error obtaining the WNS Channel URI, make sure you've replaced the zero'ed out remote id (near the top of the sample) with your own remote id.
