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
   * To obtain an Azure AppId, follow "Configure your app's identity in Azure Active Directory" [here](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push/push-quickstart#configure-your-apps-identity-in-azure-active-directory).
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
Follow these [instructions]() to send a notification to the sample. 
   * Leave the sample running to receive notifications in the foreground.
   * Close the sample to receive notifications in the background.

## Packaged vs. Unpackaged Apps Considerations
In addition to making the right API calls, WindowsAppSDK Apps that want to receive Push Notifications must correctly configure their project.
The type of configuration will depend upon whether the App is packaged or unpackaged.

### Unpackaged App
```xml
<PackagedType>none</PackagedType>
```

ddlm dll

### Packaged App
appxmanifest Extension

COM Server

```xml
<Extensions>
  <uap:Extension Category="windows.protocol" EntryPoint="Windows.FullTrustApplication">
    <uap:Protocol Name="windowsappsdktestprotocol-packaged">
      <uap:DisplayName>Windows App SDK TestProtocol</uap:DisplayName>
    </uap:Protocol>
  </uap:Extension>
  <com:Extension Category="windows.comServer">
    <com:ComServer>
      <com:ExeServer Executable="PushNotificationsTestApp\PushNotificationsTestApp.exe" DisplayName="SampleBackgroundApp" Arguments="----WindowsAppSDKPushServer">
        <com:Class Id="[Your app's Azure AppId]" DisplayName="Windows App SDK Push" />
      </com:ExeServer>
    </com:ComServer>
  </com:Extension>
</Extensions>    
```

Take notice that the Packaged sample has a special extension to the COM server and protocol used to deliver the notification to the app.

---
**How does an upackaged sample get the notification?**

*If the packaged sample requires the extensions for the COM server and protocol in order to receive notifications, how does the "unpackaged" sample manage to receive notifications as it does not have an appxmanifest file?*

## Related Links
- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Push Notifications Doc](https://docs.microsoft.com/windows/apps/windows-app-sdk/notifications/push/)

## Common Issues
* If there was an error obtaining the WNS Channel URI, make sure you've replaced the zero'ed out remote id (near the top of the sample) with your own remote id.
