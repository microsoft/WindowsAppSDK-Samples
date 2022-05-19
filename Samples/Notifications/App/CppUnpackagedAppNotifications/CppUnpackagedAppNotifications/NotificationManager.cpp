﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "NotificationManager.h"
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
}

static const std::map<unsigned, std::function<void (winrt::AppNotificationActivatedEventArgs const&)>> c_NotificationHandlers
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
            c_NotificationHandlers.at(scenarioId.value())(notificationActivatedEventArgs); // Will throw if out of range
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
