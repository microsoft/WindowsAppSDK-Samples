// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>

struct PushToastWithAvatar
{
public:
    static const unsigned ScenarioId{ 1 };
    static const wchar_t* ScenarioName;

    static const winrt::guid RemoteId;

    static bool SendToast();
    static void NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);
    static winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel RequestChannel();
    static winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel> RequestChannelAsync();
};
