// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if false
//#include "pch.h"
//#include "ToastWithAvatar.h"
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

class ToastWithAvatar
{
    public const int ScenarioId = 1;
    public const string ScenarioName = "Local Toast with Avatar Image";

    public static bool SendToast()
    {
        // The ScenarioIdToken uniquely identify a scenario and is used to route the response received when the user clicks on a toast to the correct scenario.
        var ScenarioIdToken = Common.MakeScenarioIdToken(ScenarioId);
	
        string xmlPayload = new string(
            "<toast>"
        +       "<visual>"
        +           "<binding template = \"ToastGeneric\">"
        +               "<image placement = \"appLogoOverride\" hint-crop=\"circle\" src = \"" + App.GetFullPathToAsset("Square150x150Logo.png") + "\"/>"
        +               "<text>" + ScenarioName + "</text>"
        +               "<text>This is an example message using XML</text>"
        +           "</binding>"
        +       "</visual>"
        +       "<actions>"
        +           "<action "
        +               "content = \"Open App\" "
        +               "arguments = \"action=OpenApp&amp;" + ScenarioIdToken + "\"/>"
        +       "</actions>"
        +   "</toast>" );

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
        var notification = new MainPage.Notification();
        notification.Originator = ScenarioName;
        var action = Common.ExtractParamFromArgs(notificationActivatedEventArgs.Argument, "action");
        notification.Action = action == null ? "" : action;
        MainPage.Current.NotificationReceived(notification);
    }
}
