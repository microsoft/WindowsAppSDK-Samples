// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_SelfContainedDeployment.xaml.h"
#if __has_include("Scenario1_SelfContainedDeployment.g.cpp")
#include "Scenario1_SelfContainedDeployment.g.cpp"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <shobjidl_core.h>
#include <wil/result.h>
#include <iostream>
#include <wil/win32_helpers.h>
#include <windows.h>
#endif


namespace winrt
{
    using namespace winrt::Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::UI::Xaml::Controls;
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Microsoft::Windows::AppNotifications;
    using namespace winrt::Microsoft::Windows::PushNotifications;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Windows::Foundation::Collections;
}

namespace winrt::SelfContainedDeployment::implementation
{
    MainPage Scenario1_SelfContainedDeployment::rootPage{ nullptr };

    Scenario1_SelfContainedDeployment::Scenario1_SelfContainedDeployment()
    {
        InitializeComponent();
        Scenario1_SelfContainedDeployment::rootPage = MainPage::Current();
    }

    winrt::PushNotificationChannel Scenario1_SelfContainedDeployment::RequestChannel()
    {
        auto task = RequestChannelAsync();
        if (task.wait_for(std::chrono::seconds(300)) != winrt::AsyncStatus::Completed)
        {
            task.Cancel();
            return nullptr;
        }

        auto result = task.GetResults();
        return result;
    }

    winrt::IAsyncOperation<winrt::PushNotificationChannel> Scenario1_SelfContainedDeployment::RequestChannelAsync()
    {
        // To obtain an AAD RemoteIdentifier for your app,
        // follow the instructions on https://docs.microsoft.com/azure/active-directory/develop/quickstart-register-app
        auto channelOperation = winrt::PushNotificationManager::Default().CreateChannelAsync(
            winrt::guid("cea1342d-8293-4acb-b18a-ed8b0d6f7d6c")).get();

        //  auto result = co_await channelOperation;

        if (channelOperation.Status() == winrt::PushNotificationChannelStatus::CompletedSuccess)
        {
            auto channelUri{ channelOperation.Channel().Uri() };

            std::cout << "channelUri: " << winrt::to_string(channelUri.ToString()) << std::endl << std::endl;

            auto channelExpiry = channelOperation.Channel().ExpirationTime();

            // Caller's responsibility to keep the channel alive
            co_return channelOperation.Channel();
        }
        else if (channelOperation.Status() == winrt::PushNotificationChannelStatus::CompletedFailure)
        {
            // LOG_HR_MSG(result.ExtendedError(), "We hit a critical non-retryable error with channel request!");
            std::cout << "We hit a critical non-retryable error with channel request!";
            co_return nullptr;
        }
        else
        {
            // LOG_HR_MSG(result.ExtendedError(), "Some other failure occurred.");
            std::cout << "Some other failure occurred.";
            co_return nullptr;
        }

    };

    void Scenario1_SelfContainedDeployment::RequestChannel_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_channel = RequestChannel();
        auto channelUri{ winrt::to_hstring(m_channel.Uri().ToString() + L"\n").c_str()};
        OutputDebugString(channelUri);

        rootPage.NotifyUser(channelUri, InfoBarSeverity::Success);
    }

    void Scenario1_SelfContainedDeployment::RegisterManager_Click(IInspectable const&, RoutedEventArgs const&)
    {
        THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"TestPushAppId3")); // elx - need to get a proper app id

        winrt::PushNotificationManager pushManager{ winrt::PushNotificationManager::Default() };
        winrt::AppNotificationManager appManager{ winrt::AppNotificationManager::Default() };

        pushManager.Register();
        appManager.Register();

        rootPage.NotifyUser(L"AppNotificationManager Registered COM Activator successful!", InfoBarSeverity::Success);
    }
    void Scenario1_SelfContainedDeployment::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
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

    void Scenario1_SelfContainedDeployment::RetrieveActivationArgs_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
        ExtendedActivationKind kind = args.Kind();
        if (kind == ExtendedActivationKind::Launch)
        {
            rootPage.NotifyUser(L"ExtendedActivationKind::Launch", InfoBarSeverity::Success);
        }
        else if (kind == ExtendedActivationKind::Push)
        {
            rootPage.NotifyUser(L"ExtendedActivationKind::Push", InfoBarSeverity::Success);
        }
        else if (kind == ExtendedActivationKind::AppNotification)
        {
            rootPage.NotifyUser(L"ExtendedActivationKind::AppNotification", InfoBarSeverity::Success);
        }
        else
        {
            rootPage.NotifyUser(L"Unhandled activation type", InfoBarSeverity::Success);
        }
    }
}
