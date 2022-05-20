// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "NotificationManager.h"
#include "App.xaml.h"
#include "MainPage.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"
#include "Common.h"
#include "NotifyUser.h"
#include <map>
#include <functional>

namespace winrt
{
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::Windows::AppNotifications;
    using namespace CppUnpackagedAppNotifications::implementation;
}

static const std::map<unsigned, std::function<void (winrt::AppNotificationActivatedEventArgs const&)>> c_notificationHandlers
{
    // When adding new a scenario, be sure to add its notification handler here.
    { ToastWithAvatar::ScenarioId, ToastWithAvatar::NotificationReceived },
    { ToastWithTextBox::ScenarioId, ToastWithTextBox::NotificationReceived }
};

NotificationManager::NotificationManager():m_isRegistered(false){}

NotificationManager::~NotificationManager()
{
    if (m_isRegistered)
    {
        winrt::AppNotificationManager::Default().Unregister();
    }
}

void NotificationManager::Init()
{
    auto notificationManager{ winrt::AppNotificationManager::Default() };

    // Always setup the notification hanlder before registering your App, otherwise notifications may get lost.
    const auto token{ notificationManager.NotificationInvoked([&](const auto&, winrt::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
        {
            NotifyUser::NotificationReceived();

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                NotifyUser::UnrecognizedToastOriginator();
            }
        }) };

    winrt::AppNotificationManager::Default().Register();
    m_isRegistered = true;
}

void NotificationManager::ProcessLaunchActivationArgs(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    assert(m_isRegistered);

    DispatchNotification(notificationActivatedEventArgs);
    NotifyUser::AppLaunchedFromNotification();
}

bool NotificationManager::DispatchNotification(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto scenarioId{ Common::ExtractScenarioIdFromArgs(notificationActivatedEventArgs.Argument().c_str())};
    if (scenarioId.has_value())
    {
        try
        {
            c_notificationHandlers.at(scenarioId.value())(notificationActivatedEventArgs); // Will throw if out of range
            return true;
        }
        catch (...)
        {
            return false; // Couldn't find a NotificationHandler for scenarioId.
        }
    }
    else
    {
        NotificationManager::HandleBackgroundClick(); // User has clicked on the background of the toast instead of taking a specific action (like clicking a button).
        return true;
    }
}

void NotificationManager::HandleBackgroundClick()
{
    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = L"Local Toast: no specific scenario";
    notification.Action = L"background click";
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}
