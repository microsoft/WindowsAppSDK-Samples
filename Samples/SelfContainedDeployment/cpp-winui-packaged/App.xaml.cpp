// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::PushNotifications;
    using namespace Microsoft::Windows::AppNotifications;
}

namespace winrt::SelfContainedDeployment::implementation
{
    App::App()
    {
        InitializeComponent();

        /*winrt::PushNotificationManager pushManager{ winrt::PushNotificationManager::Default() };
        winrt::AppNotificationManager appManager{ winrt::AppNotificationManager::Default() };

        pushManager.Register();
        appManager.Register();*/
    }

    void App::OnLaunched(winrt::LaunchActivatedEventArgs const&)
    {
        window = winrt::make<MainWindow>();
        window.Activate();
    }
}
