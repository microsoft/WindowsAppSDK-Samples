// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "Notifications\NotificationManager.h"
#include <wil/result.h>
#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Windows.ApplicationModel.Background.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::AppLifecycle;
    using namespace Microsoft::Windows::AppNotifications;
    using namespace Microsoft::Windows::PushNotifications;
}

// NotificationManager is responsible for registering and unregistering the Sample for App Notifications as well as
// dispatching actioned notifications to the appropriate scenario.
// Registration will happen when Init() is called and Unregistration will happen when this
// instance variable goes out of scope, i.e.: when the App is terminated.
static NotificationManager g_notificationManager;

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    static App* app{ nullptr };

    App::App()
    {
        g_notificationManager.Init();

        InitializeComponent();
        app = this;

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

    void App::ToForeground()
    {
        assert(app != nullptr);

        HWND hwnd;
        auto windowNative{ app->window.as<IWindowNative>() };
        if (windowNative && SUCCEEDED(windowNative->get_WindowHandle(&hwnd)))
        {
            SwitchToThisWindow(hwnd, TRUE);
        }
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
                switch (activationArgs.Kind())
                {
                    // When it is activated from a push notification, the sample only displays the notification.
                    // It doesn’t register for foreground activation of perform any other actions
                    // because background activation is meant to let app perform only small tasks in order to preserve battery life.
                case ExtendedActivationKind::Push:
                {
                    winrt::PushNotificationReceivedEventArgs pushArgs{ activationArgs.Data().as<winrt::PushNotificationReceivedEventArgs>() };

                    // Call GetDeferral to ensure that code runs in low power
                    auto deferral{ pushArgs.GetDeferral() };

                    auto payload{ pushArgs.Payload() };

                    // Do stuff to process the raw notification payload
                    std::string payloadString(payload.begin(), payload.end());
                    //std::cout << "\nPush notification content received in the BACKGROUND: " << payloadString.c_str() << std::endl;
                    //std::cout << "\nPress 'Enter' to exit the App." << std::endl;

                    // Call Complete on the deferral when finished processing the payload.
                    // This removes the override that kept the app running even when the system was in a low power mode.
                    deferral.Complete();
                    //std::cin.ignore();
                }
                break;

                case winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind::AppNotification:
                {
                    winrt::AppNotificationActivatedEventArgs notificationActivatedEventArgs{ activationArgs.Data().as<winrt::AppNotificationActivatedEventArgs>() };
                    g_notificationManager.ProcessLaunchActivationArgs(notificationActivatedEventArgs);
                }
                break;
                }
            }
        }

        window.Activate();
    }
}
