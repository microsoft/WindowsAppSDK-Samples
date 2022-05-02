﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "NotificationManager.h"
#include <wil/result.h>
#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Microsoft::Windows::AppNotifications;
}

static NotificationManager g_notificationManager;

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    App::App()
    {
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

    std::wstring App::GetFullPathToExe()
    {
        TCHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
        return std::wstring(buffer).substr(0, pos);
    }

    std::wstring App::GetFullPathToAsset(std::wstring const& assetName)
    {
        return GetFullPathToExe() + L"\\Assets\\" + assetName;
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& /*args*/)
    {
        window = make<MainWindow>();

        g_notificationManager.Init();

        // NOTE: AppInstance is ambiguous between
        // Microsoft.Windows.AppLifecycle.AppInstance and
        // Windows.ApplicationModel.AppInstance
        auto currentInstance{ winrt::AppInstance::GetCurrent() };
        if (currentInstance)
        {
            // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
            // even in WinUI's OnLaunched.
            winrt::AppActivationArguments activationArgs{ currentInstance.GetActivatedEventArgs() };
            if (activationArgs)
            {
                winrt::ExtendedActivationKind extendedKind{ activationArgs.Kind() };
                if (extendedKind == winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind::AppNotification)
                {
                    winrt::AppNotificationActivatedEventArgs notificationActivatedEventArgs{ activationArgs.Data().as<winrt::AppNotificationActivatedEventArgs>() };
                    g_notificationManager.ProcessLaunchActivationArgs(notificationActivatedEventArgs);
                }


            }
        }

        window.Activate();
    }
}