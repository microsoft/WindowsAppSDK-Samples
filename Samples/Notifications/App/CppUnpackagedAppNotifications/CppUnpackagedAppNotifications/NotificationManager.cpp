#include "pch.h"
#include "NotificationManager.h"
#include "MainPage.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml::Controls;
}

NotificationManager::NotificationManager():m_isRegistered(false){}

NotificationManager::~NotificationManager()
{
    if (m_isRegistered)
    {
        winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Unregister();
    }
}

void NotificationManager::Init()
{
    auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
    const auto token{ notificationManager.NotificationInvoked([&](const auto&, winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
        {
            winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Notification received", winrt::InfoBarSeverity::Informational);

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", winrt::InfoBarSeverity::Error);
            }
        }) };

    winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Register();
    m_isRegistered = true;

}

bool NotificationManager::DispatchNotification(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
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
