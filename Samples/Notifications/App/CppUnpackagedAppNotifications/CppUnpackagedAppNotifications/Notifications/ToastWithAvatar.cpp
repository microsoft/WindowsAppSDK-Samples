// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ToastWithAvatar.h"
#include "Common.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>
#include <winrt/Windows.Foundation.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

namespace winrt
{
    using namespace Microsoft::Windows::AppNotifications;
    using namespace Microsoft::Windows::AppNotifications::Builder;
    using namespace CppUnpackagedAppNotifications::implementation;
}

const wchar_t* ToastWithAvatar::ScenarioName{ L"Local Toast with Avatar Image" };

bool ToastWithAvatar::SendToast()
{
    auto appNotification{ winrt::AppNotificationBuilder()
        .AddArgument(L"action", L"ToastClick")
        .AddArgument(Common::scenarioTag, std::to_wstring(ToastWithAvatar::ScenarioId))
        .SetAppLogoOverride(winrt::Windows::Foundation::Uri(L"file://" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png")), winrt::AppNotificationImageCrop::Circle)
        .AddText(ScenarioName)
        .AddText(L"This is an example message using XML")
        .AddButton(winrt::AppNotificationButton(L"Open App")
            .AddArgument(L"action", L"OpenApp")
            .AddArgument(Common::scenarioTag, std::to_wstring(ToastWithAvatar::ScenarioId)))
        .BuildNotification() };

    winrt::AppNotificationManager::Default().Show(appNotification);

    return appNotification.Id() != 0; // return true (indicating success) if the toast was sent (if it has an Id)
}

void ToastWithAvatar::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = ScenarioName;
    notification.Action = notificationActivatedEventArgs.Arguments().Lookup(L"action");
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}
