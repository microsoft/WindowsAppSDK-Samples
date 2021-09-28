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
This sample demonstrates 
1. How an unpackaged app can use the WindowsAppSDK to request a channel URI
    * The Channel URI uniquely identifies the app for a current user on a specific device, allowing other apps and servers to send notifications to this app.
2.	How to use the WindowsAppSDK to register an app for foreground and background activation.
The Code
When it is launched normally (by the users, or from the debugger), the sample requests a channeluri and displays it, then waits for notifications.
    * This allows to copy the channeluri and sent notifications
When it is activated from a push notification, the sample displays the notification.
    * It doesn’t register for foreground activation of perform any other actions.
       * Because background activation is meant to let app perform only small tasks in order to preserve battery life.
## Requirements
The sample requires a web API tool like Postman or fiddler to send notifications.
* The WindowsAppSDK push notifications APIs defines all that is needed to register for and receive notifications but it's the role of an external app to send the notifications.
This ... (postman, fiddler) is used to eimulate that external server and sends notifications.

## Building and running any of the samples 
Before building the sample, make sure to set up your environment correctly by following the steps [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
For the unpackaged sample apps, in addition to the steps for setting up the environment for the Windows App SDK, make sure to follow the instructions [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/deploy-unpackaged-apps).

1. Open the solution file (.sln) in the subfolder of your preferred sample in Visual Studio 2019 Preview.
2. Build the solution.
3. Right click on the solution and click **Deploy Solution**
4. Debug the project.

## Receiving Notifications
### Packaged
1.	Obtain an RemoteId for your sample
    * The remoteId (and corresponding secret) allows a publisher to authenticate themselves with the WNS server.
    * The WNS server receives notifications from a publisher and dispatches it to the appropriate app for the appropriate user on the appropriate device.
2.	Prepare to send a notification
3.	Build the sample
4.	Run the sample and copy the channeluri
    * The ChannelUri uniquely identify the app, user, and device.
    * WNS uses the channel URI to route notifications to the appropriate app, user, and device combination.
5.	Send a notification
6.	The sample should have received it as a foreground activation
7.	Close the sample app
8.	Send another notification
9.	The sample should have been activated from the background.
### Unpackaged
## Low power mode activity
When waiting for push toast, the app uses deferred mode 
* This ensures that notifications will be received even if the device enters low power mode.

## Related Links
- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Push Notifications Doc]()
