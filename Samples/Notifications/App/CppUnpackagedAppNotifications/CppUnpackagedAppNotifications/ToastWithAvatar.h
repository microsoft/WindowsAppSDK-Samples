#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

class ToastWithAvatar
{
public:
    static bool SendToast();
    static void NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs);
};

