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
    using namespace Microsoft::Windows::PushNotifications;
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
    auto pushNotificationManager = winrt::PushNotificationManager::Default();

    if (pushNotificationManager.IsSupported())
    {
        // Setup an event handler, so we can receive notifications in the foreground while the app is running.
        SubscribeForegroundEventHandler();

        pushNotificationManager.Register();
    }
    else
    {
        // Here, the app should handle the case where push notifications are not supported, for example:
        // - maintain its own persistent connection with an App Service or
        // - use polling over a scheduled interval to synchronize the client.
        //std::cout << "\nPush Notifications aren't supported." << std::endl;
    }

    auto appNotificationManager{ winrt::AppNotificationManager::Default() };

    // To ensure all Notification handling happens in this process instance, register for
    // NotificationInvoked before calling Register(). Without this a new process will
    // be launched to handle the notification.
    const auto token{ appNotificationManager.NotificationInvoked([&](const auto&, winrt::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
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
    DispatchNotification(notificationActivatedEventArgs);
    NotifyUser::AppLaunchedFromNotification();
}

bool NotificationManager::DispatchNotification(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto scenarioId{ notificationActivatedEventArgs.Arguments().Lookup(Common::scenarioTag) };
    if (!scenarioId.empty())
    {
        try
        {
            c_notificationHandlers.at(std::stoul(std::wstring(scenarioId.c_str())))(notificationActivatedEventArgs); // Will throw if out of range
            return true;
        }
        catch (...)
        {
            return false; // Couldn't find a NotificationHandler for scenarioId.
        }
    }
    else
    {
        return false; // No scenario specified in the notification
    }
}

void NotificationManager::SubscribeForegroundEventHandler()
{
    winrt::event_token token{ winrt::PushNotificationManager::Default().PushReceived([](auto const&, winrt::PushNotificationReceivedEventArgs const& args)
    {
        auto payload{ args.Payload() };

        std::string payloadString(payload.begin(), payload.end());
        //std::cout << "\nPush notification content received in the FOREGROUND: " << payloadString << std::endl;
    }) };
}
