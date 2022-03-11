// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <wil/result.h>
#include <wil/cppwinrt.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <iostream>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <windows.h>
#include <shobjidl_core.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Windows::ApplicationModel::Activation;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Microsoft::Windows::AppNotifications;
    using namespace winrt::Windows::Storage;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::Storage::Streams;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    App::App()
    {
        winrt::ActivationRegistrationManager::RegisterForStartupActivation(
            L"StartupId",
            L""
        );

        InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](winrt::IInspectable const&, winrt::UnhandledExceptionEventArgs const& e)
            {
                if (IsDebuggerPresent())
                {
                    auto errorMessage = e.Message();
                    __debugbreak();
                }
            });
#endif
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        window = winrt::make<MainWindow>();
        window.Activate();

        THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"TestPushAppId3")); // elx - need to get a proper app id

        winrt::event_token token = winrt::AppNotificationManager::Default().NotificationInvoked([](const auto&, winrt::AppNotificationActivatedEventArgs const& toastArgs)
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

        winrt::AppNotificationManager::Default().Register();

        //auto args = winrt::AppInstance::GetCurrent().GetActivatedEventArgs();
        //auto kind = args.Kind();
    }
}
