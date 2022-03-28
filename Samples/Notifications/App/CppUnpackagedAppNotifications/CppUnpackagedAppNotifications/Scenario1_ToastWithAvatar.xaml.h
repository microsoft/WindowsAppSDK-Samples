// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ToastWithAvatar.g.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar>
    {
        Scenario1_ToastWithAvatar();

        void Notification_received(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs);

        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void GetActivationInfo_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppUnpackagedAppNotifications::MainPage rootPage;
    };
}

namespace winrt::CppUnpackagedAppNotifications::factory_implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar, implementation::Scenario1_ToastWithAvatar>
    {
    };
}

void OutputMessage(const WCHAR* message);
void OutputFormattedMessage(const WCHAR* fmt, ...);
std::vector<std::wstring> split_strings(winrt::hstring argString);
void GetActivationInfo();

extern winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> messages;
