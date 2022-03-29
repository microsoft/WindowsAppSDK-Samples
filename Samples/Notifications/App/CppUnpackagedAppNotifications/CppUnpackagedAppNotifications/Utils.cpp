#include "pch.h"
#include "Utils.h"
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"

bool Utils::DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };
    if (args.find(L"activateToast") != std::wstring::npos)
    {
        ToastWithAvatar::NotificationReceived(notificationActivatedEventArgs);
    }
    else if (args.find(L"reply") != std::wstring::npos)
    {
        ToastWithTextBox::NotificationReceived(notificationActivatedEventArgs);
    }
    else
    {
        return false;
    }

    return true;
}

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

std::wstring Utils::GetFullPathToAsset(std::wstring const& assetName)
{
    return ExePath() + L"\\Assets\\" + assetName;
}
