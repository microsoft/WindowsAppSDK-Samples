// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_SelfContainedDeployment.g.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::SelfContainedDeployment::implementation
{
    struct Scenario1_SelfContainedDeployment : Scenario1_SelfContainedDeploymentT<Scenario1_SelfContainedDeployment>
    {
        Scenario1_SelfContainedDeployment();
        
        void RequestChannel_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void RegisterManager_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void RetrieveActivationArgs_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        winrt::Microsoft::Windows::AppNotifications::AppNotificationManager m_appNotificationManager {nullptr};
        winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel m_channel {nullptr};

        winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel RequestChannel();
        winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel> RequestChannelAsync();
    private:
        static SelfContainedDeployment::MainPage rootPage;
    };
}

namespace winrt::SelfContainedDeployment::factory_implementation
{
    struct Scenario1_SelfContainedDeployment : Scenario1_SelfContainedDeploymentT<Scenario1_SelfContainedDeployment, implementation::Scenario1_SelfContainedDeployment>
    {
    };
}
