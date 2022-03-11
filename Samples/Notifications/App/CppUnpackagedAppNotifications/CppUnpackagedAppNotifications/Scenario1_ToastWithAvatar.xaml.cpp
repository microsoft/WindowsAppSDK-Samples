// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
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

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario1_ToastWithAvatar::rootPage{ nullptr };

    Scenario1_ToastWithAvatar::Scenario1_ToastWithAvatar()
    {
        InitializeComponent();
        Scenario1_ToastWithAvatar::rootPage = MainPage::Current();

        m_appNotificationManager =  winrt::AppNotificationManager::Default();
    }

    winrt::IAsyncOperation<winrt::PushNotificationChannel> Scenario1_ToastWithAvatar::RequestChannelAsync()
    {
        // To obtain an AAD RemoteIdentifier for your app,
        // follow the instructions on https://docs.microsoft.com/azure/active-directory/develop/quickstart-register-app
        auto channelOperation = winrt::PushNotificationManager::Default().CreateChannelAsync(
            winrt::guid("0160ee84-0c53-4851-9ff2-d7f5a87ed914")).get();

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

    winrt::PushNotificationChannel Scenario1_ToastWithAvatar::RequestChannel()
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

    void Scenario1_ToastWithAvatar::RequestChannel_Click(IInspectable const&, RoutedEventArgs const&)
    {
       m_channel = RequestChannel();

        rootPage.NotifyUser(winrt::to_hstring(m_channel.Uri().ToString()).c_str(), InfoBarSeverity::Success);
    }

    void Scenario1_ToastWithAvatar::RegisterManager_Click(IInspectable const&, RoutedEventArgs const&)
    {
        THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"TestPushAppId3")); // elx - need to get a proper app id

        winrt::event_token token = m_appNotificationManager.NotificationInvoked([](const auto&, winrt::AppNotificationActivatedEventArgs const& toastArgs)
            {
                std::wcout << L"AppNotification received foreground!\n";
                winrt::hstring arguments{ toastArgs.Argument() };
                std::wcout << arguments.c_str() << L"\n\n";

                OutputDebugString(L"Foreground Activated");

                winrt::IMap<winrt::hstring, winrt::hstring> userInput{ toastArgs.UserInput() };
                for (auto pair : userInput)
                {
                    std::wcout << "Key= " << pair.Key().c_str() << " " << "Value= " << pair.Value().c_str() << L"\n";
                }
                std::wcout << L"\n";
            });

        m_appNotificationManager.Register();

        rootPage.NotifyUser(L"AppNotificationManager Registered COM Activator successful!", InfoBarSeverity::Success);
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

    void Scenario1_ToastWithAvatar::RetrieveActivationArgs_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
        ExtendedActivationKind kind = args.Kind();
        if (kind == ExtendedActivationKind::Launch)
        {
            OutputDebugString(L"ExtendedActivationKind::Launch");
        }
        else if (kind == ExtendedActivationKind::Protocol)
        {
            OutputDebugString(L"ExtendedActivationKind::Protocol");
        }
        else if (kind == ExtendedActivationKind::Push)
        {
            OutputDebugString(L"ExtendedActivationKind::Push");
        }
        else if (kind == ExtendedActivationKind::AppNotification)
        {
            OutputDebugString(L"ExtendedActivationKind::AppNotification");
        }
        else if (kind == ExtendedActivationKind::ToastNotification)
        {
            OutputDebugString(L"ExtendedActivationKind::ToastNotification");
        }
        else
        {
            OutputDebugString(L"LOL");
        }
    }
}
