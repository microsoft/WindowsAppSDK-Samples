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
    public static bool SendToast()
    {
        string xmlPayload = new string(
            "<toast>"
        +       "<visual>"
        +           "<binding template = \"ToastGeneric\">"        +               "<image placement = \"appLogoOverride\" src = \"" + App.GetFullPathToAsset("Square150x150Logo.png") + "\"/>"        +               "<text>App Notifications Sample Scenario 1</text>"
        +               "<text>This is an example message using XML</text>"
        +           "</binding>"
        +       "</visual>"
        +       "<actions>"
        +           "<action "
        +               "content = \"Activate Toast\" "        +               "arguments = \"action=activateToast&amp;scenarioId=1\" "        +               "activationType = \"foreground\"/>"
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
#if false
    void NotificationReceived(Microsoft.Windows.AppNotifications.AppNotificationActivatedEventArgs notificationActivatedEventArgs)
    {
        CsUnpackagedAppNotifications.Notification notification{};
        notification.Originator = L"Scenario1_ToastWithAvatar";
        auto action{ Common::ExtractParam(notificationActivatedEventArgs.Argument().c_str(), L"action") };
        notification.Action = action.has_value() ? action.value() : L"";
        MainPage.Current.NotificationReceived(notification);
    }
#endif
}
