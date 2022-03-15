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
This sample demonstrates
1. How an app can use the WindowsAppSDK to request a WNS ChannelURI
    * The WNS ChannelURI uniquely identifies the app for a current user on a specific device, allowing other apps and servers to send notifications to this app.
2.	How to use the WindowsAppSDK to register an app for foreground and background activation and handle the activations.
## Requirements
* A web API tool (like Postman or fiddler) to send notifications.
   * The WindowsAppSDK push notifications APIs provides everything an app needs register for and receive notifications but it's expected that an external server app will be sending the notifications. The web API tool is used to emulate that external server app to send notifications.
* An AAD RemoteIdentifier for your app.
   * To obtain an AAD Remote identifier, follow these [instructions](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/notifications/push/push-quickstart#configure-your-apps-identity-in-azure-active-directory).
* Windows 10
## Building and running the sample 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).
### To build the sample
1. Open the solution file (.sln) in the subfolder of the sample in Visual Studio.
2. Replace the guid in the following line (near the top of the sample) with your own AAD RemoteIdentifer:
``` cpp
winrt::guid remoteId{ "00000000-0000-0000-0000-000000000000"}; 
```
3. Build the solution. Press **Ctrl+Shift+B**, or select **Build** \> **Build Solution**.
4. Debug the project. Press **F5**, or select **Debug** \> **Start Debugging**. 

The sample will display a WNS ChannelURI which uniquely identify this app, for this user on this device and thus can be used by other apps (usually a server app) to send notifications to this specific app.
## Receiving Notifications
Follow these [instructions]() to send a notification to the sample. 
   * Leave the sample running to receive notifications in the foreground.
   * Close the sample to receive notifications in the background.
## Related Links
- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Push Notifications Doc]()
## Common Issues
* If there was an error obtaining the WNS ChannelURI, make sure you've replaced the zero'ed out remote id (near the top of the sample) with your own remote id.
