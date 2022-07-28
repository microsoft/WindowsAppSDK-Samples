// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ToastWithAvatar.h"
#include "Common.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

namespace winrt
{
    using namespace Microsoft::Windows::AppNotifications;
    using namespace CppUnpackagedAppNotifications::implementation;
}

const wchar_t* ToastWithAvatar::ScenarioName{ L"Local Toast with Avatar Image" };

bool ToastWithAvatar::SendToast()
{
    // The ScenarioIdToken uniquely identify a scenario and is used to route the response received when the user clicks on a toast to the correct scenario.
    auto ScenarioIdToken{ Common::MakeScenarioIdToken(ToastWithAvatar::ScenarioId) };

    winrt::hstring xmlPayload{
        L"<toast launch = \"action=ToastClick&amp;" + ScenarioIdToken + L"\">\
            <visual>\
                <binding template = \"ToastGeneric\">\
                    <image placement = \"appLogoOverride\" hint-crop=\"circle\" src = \"" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png") + L"\"/>\
                    <text>" + ScenarioName + L"</text>\
                    <text>This is an example message using XML</text>\
                </binding>\
            </visual>\
            <actions>\
                <action\
                    content = \"Open App\"\
                    arguments = \"action=OpenApp&amp;" + ScenarioIdToken + L"\"/>\
            </actions>\
        </toast>" };

    auto toast{ winrt::AppNotification(xmlPayload) };
    winrt::AppNotificationManager::Default().Show(toast);

    return toast.Id() != 0; // return true (indicating success) if the toast was sent (if it has an Id)
}

void ToastWithAvatar::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = ScenarioName;
    auto action{ Common::ExtractParamFromArgs(notificationActivatedEventArgs.Argument().c_str(), L"action") };
    notification.Action = action.has_value() ? action.value() : L"";
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}
