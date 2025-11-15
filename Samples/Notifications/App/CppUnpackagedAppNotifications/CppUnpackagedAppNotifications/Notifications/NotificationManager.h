// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>

class NotificationManager
{
public:
    NotificationManager();
    ~NotificationManager();

    void Init();
    void ProcessLaunchActivationArgs(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);

 private:
     bool DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);
     void SubscribeForegroundEventHandler();

     bool m_isRegistered;
};
