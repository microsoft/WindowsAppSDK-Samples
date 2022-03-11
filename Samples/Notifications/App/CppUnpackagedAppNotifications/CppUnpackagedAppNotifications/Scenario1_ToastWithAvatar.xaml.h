// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ToastWithAvatar.g.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar>
    {
        Scenario1_ToastWithAvatar();

        void RequestChannel_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void RegisterManager_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void RetrieveActivationArgs_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppUnpackagedAppNotifications::MainPage rootPage;
        winrt::Microsoft::Windows::AppNotifications::AppNotificationManager m_appNotificationManager {nullptr};
        winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel m_channel {nullptr};

        winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel RequestChannel();
        winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel> RequestChannelAsync();
    };
}

namespace winrt::CppUnpackagedAppNotifications::factory_implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar, implementation::Scenario1_ToastWithAvatar>
    {
    };
}
