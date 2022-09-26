// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ToastWithTextBox.h"
#include "Common.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

namespace winrt
{
    using namespace Microsoft::Windows::AppNotifications;
    using namespace Microsoft::Windows::AppNotifications::Builder;
    using namespace CppUnpackagedAppNotifications::implementation;
}

const wchar_t* ToastWithTextBox::ScenarioName{ L"Local Toast with Avatar and Text Box" };
auto textboxReplyId{ L"textboxReply" };

bool ToastWithTextBox::SendToast()
{
    auto appNotification{ winrt::AppNotificationBuilder()
        .AddArgument(L"action", L"ToastClick")
        .AddArgument(Common::scenarioTag, std::to_wstring(ToastWithTextBox::ScenarioId))
        .SetAppLogoOverride(winrt::Windows::Foundation::Uri(L"file://" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png")), winrt::AppNotificationImageCrop::Circle)
        .AddText(ScenarioName)
        .AddText(L"This is an example message using XML")
        .AddTextBox(textboxReplyId, L"Type a reply", L"Reply box")
        .AddButton(winrt::AppNotificationButton(L"Reply")
            .AddArgument(L"action", L"Reply")
            .AddArgument(Common::scenarioTag, std::to_wstring(ToastWithTextBox::ScenarioId))
            .SetInputId(textboxReplyId))
        .BuildNotification() };

    winrt::AppNotificationManager::Default().Show(appNotification);

    return appNotification.Id() != 0; // return true (indicating success) if the toast was sent (if it has an Id)
}

void ToastWithTextBox::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    // In a real-life scenario, this type of action would usually be processed in the background. Your App would process the payload in
    // the background (sending the payload back to your App Server) without ever showing the App's UI.
    // This is not something that can easily be demonstrated in a sample such as this one, as we need to show the UI to demonstrate how
    // the payload is routed internally



    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = ScenarioName;
    notification.Action = notificationActivatedEventArgs.Arguments().Lookup(L"action");
    notification.HasInput = true;
    notification.Input = notificationActivatedEventArgs.UserInput().Lookup(textboxReplyId);
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}
