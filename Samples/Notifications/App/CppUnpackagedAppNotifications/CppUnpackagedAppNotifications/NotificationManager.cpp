﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "NotificationManager.h"
#include "MainPage.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"
#include "Common.h"

#include <map>
#include <functional>

namespace winrt
{
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::Windows::AppNotifications;
}

static const std::map<unsigned, std::function<void (winrt::AppNotificationActivatedEventArgs const&)>> c_map
{
    { ToastWithAvatar::ScenarioId, ToastWithAvatar::NotificationReceived },
    { ToastWithAvatar::ScenarioId, ToastWithTextBox::NotificationReceived }
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
    const auto token{ notificationManager.NotificationInvoked([&](const auto&, winrt::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
        {
            winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Notification received", winrt::InfoBarSeverity::Informational);

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", winrt::InfoBarSeverity::Error);
            }
        }) };

    winrt::AppNotificationManager::Default().Register();
    m_isRegistered = true;
}

void NotificationManager::ProcessLaunchActivationArgs(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    assert(m_isRegistered);

    DispatchNotification(notificationActivatedEventArgs);
    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"App launched from notifications", winrt::InfoBarSeverity::Informational);
}

bool NotificationManager::DispatchNotification(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto scenarioId{ Common::ExtractScenarioIdFromArgs(notificationActivatedEventArgs.Argument().c_str())};
    if (scenarioId.has_value())
    {
        try
        {
            c_map.at(scenarioId.value())(notificationActivatedEventArgs); // Will throw if out of range
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
