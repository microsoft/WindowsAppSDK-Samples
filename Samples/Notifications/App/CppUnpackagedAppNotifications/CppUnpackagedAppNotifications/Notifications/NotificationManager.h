// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

class NotificationManager
{
public:
    NotificationManager();
    ~NotificationManager();

    void Init();
    void ProcessLaunchActivationArgs(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);

 private:
     bool DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);

     bool m_isRegistered;
};
