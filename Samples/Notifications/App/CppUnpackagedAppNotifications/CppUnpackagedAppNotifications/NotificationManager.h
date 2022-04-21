#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

class NotificationManager
{
public:
    NotificationManager();
    ~NotificationManager();

    void Init();

 private:
     bool NotificationManager::DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);

     bool m_isRegistered;
};

