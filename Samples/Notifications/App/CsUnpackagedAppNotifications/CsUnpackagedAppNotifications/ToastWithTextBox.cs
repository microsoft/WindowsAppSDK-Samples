// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if Eric
//#include "pch.h"
//#include "ToastWithTextBox.h"
//#include "Common.h"
//#include <winrt/Microsoft.Windows.AppNotifications.h>
//#include "App.xaml.h"
//#include "MainPage.xaml.h"
//#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
    using namespace CppUnpackagedAppNotifications::implementation;
}
#endif

using CsUnpackagedAppNotifications;
using Microsoft.Windows.AppNotifications;

class ToastWithTextBox
{
    public static bool SendToast()
    {
        string xmlPayload = new string(
            "<toast>"
        +       "<visual>"
        +           "<binding template = \"ToastGeneric\">"
        +               "<image placement = \"appLogoOverride\" src = \"" + App.GetFullPathToAsset("Square150x150Logo.png") + "\"/>"
        +               "<text>App Notifications Sample Scenario 2</text>"
        +               "<text>This is an example message using XML</text>"
        +           "</binding>"
        +       "</visual>"
        +       "<actions>"
        +           "<input "
        +               "id = \"tbReply\" "
        +               "type = \"text\" "
        +               "placeHolderContent = \"Type a reply\"/>"
        +       "<action "
        +           "content = \"Reply\" "
        +           "id = \"something\" "
        +           "arguments = \"action=reply&amp;scenarioId=2\" "
        +           "activationType = \"background\"/>"
        +       "</actions>"
        +   "</toast>");

        AppNotification toast = new AppNotification(xmlPayload);
        toast.Priority = AppNotificationPriority.High;
        AppNotificationManager.Default.Show(toast);
        if (toast.Id == 0)
        {
            return false;
        }

        return true;
    }

    public static void NotificationReceived(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
    {
        var input = notificationActivatedEventArgs.UserInput;
        var text = input["tbReply"];
        
        var notification = new MainPage.Notification();
        notification.Originator = "Scenario2_ToastWithTextBox";
        var action = Common.ExtractParam(notificationActivatedEventArgs.Argument, "action");
        notification.Action = action == null ? "" : action;
        notification.HasInput = true;
        notification.Input = text;
        MainPage.Current.NotificationReceived(notification);
    }
}
