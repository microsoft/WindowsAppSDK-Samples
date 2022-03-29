#pragma once
#include <winrt/Microsoft.Windows.AppNotifications.h>

class Utils
{
public:
    static bool DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs);
    static std::wstring GetFullPathToAsset(std::wstring const& assetName);
};

