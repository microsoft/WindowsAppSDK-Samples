﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

// 
class ToastWithAvatar
{
public:
    static const unsigned ScenarioId{ 1 };

    ToastWithAvatar() = delete;

    static bool SendToast();
    static void NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);
};

