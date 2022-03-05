// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario1_ToastWithAvatar::rootPage{ nullptr };

    Scenario1_ToastWithAvatar::Scenario1_ToastWithAvatar()
    {
        InitializeComponent();
        Scenario1_ToastWithAvatar::rootPage = MainPage::Current();
    }
    
    void Scenario1_ToastWithAvatar::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring xmlPayload{ L"<toast>message</toast>" };
        auto toast{ winrt::AppNotification(xmlPayload) };
        toast.Priority(winrt::AppNotificationPriority::High);
        winrt::AppNotificationManager::Default().Show(toast);
        if (toast.Id() == 0)
        {
            rootPage.NotifyUser(L"Could not send toast", InfoBarSeverity::Error);
        }

        rootPage.NotifyUser(L"Toast sent successfully!", InfoBarSeverity::Success);
    }
}
