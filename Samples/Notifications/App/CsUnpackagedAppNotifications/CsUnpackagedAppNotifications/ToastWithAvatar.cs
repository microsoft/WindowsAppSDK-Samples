﻿// Copyright (c) Microsoft Corporation.
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
#if false
        winrt::hstring xmlPayload{
            L"<toast>\
                    < visual >\
                    < binding template = \"ToastGeneric\">\
                         < image placement = \"appLogoOverride\" src = \"" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png") + L"\"/>\
                          < text > App Notifications Sample Scenario 1 </ text >\
                        < text > This is an example message using XML</ text >\
                    </ binding >\
                </ visual >\
                < actions >\
                    < action\
                        content = \"Activate Toast\"\
                            arguments = \"action=activateToast&amp;scenarioId=1\"\
                            activationType = \"foreground\"/>\
                </ actions >\
            </ toast > " };
    
    auto toast{ AppNotification(xmlPayload) };
#endif

        string xmlPayload = new string(
            "<toast>"
        +       "<visual>"
        +           "<binding template = \"ToastGeneric\">"        +               "<image placement = \"appLogoOverride\" src = \"" + App.GetFullPathToAsset("Square150x150Logo.png") + "\"/>"        +                   "<text>App Notifications Sample Scenario 1</text>"
        +                   "<text>This is an example message using XML</text>"
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
    }
#if false
    void ToastWithAvatar::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = L"Scenario1_ToastWithAvatar";
    auto action{ Common::ExtractParam(notificationActivatedEventArgs.Argument().c_str(), L"action") };
    notification.Action = action.has_value() ? action.value() : L"";
    winrt::MainPage::Current().NotificationReceived(notification);
}
#endif