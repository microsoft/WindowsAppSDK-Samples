// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <wil/result.h>
#include <wil/cppwinrt.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <windows.ui.popups.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>

#include <propkey.h> //PKEY properties
#include <propsys.h>
#include <ShObjIdl_core.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Windows::ApplicationModel::Activation;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
}

// This function is intended to be called in the unpackaged scenario.
void SetDisplayNameAndIcon() noexcept try
{
    // Not mandatory, but it's highly recommended to specify AppUserModelId
    THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"ToastSampleAppId"));

    // Icon is mandatory
    winrt::com_ptr<IPropertyStore> propertyStore;
    wil::unique_hwnd hWindow{ GetConsoleWindow() };

    THROW_IF_FAILED(SHGetPropertyStoreForWindow(hWindow.get(), IID_PPV_ARGS(&propertyStore)));

    wil::unique_prop_variant propVariantIcon;
    wil::unique_cotaskmem_string sth = wil::make_unique_string<wil::unique_cotaskmem_string>(LR"(%SystemRoot%\system32\@WLOGO_96x96.png)");
    propVariantIcon.pwszVal = sth.release();
    propVariantIcon.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchIconResource, propVariantIcon));

    // App name is not mandatory, but it's highly recommended to specify it
    wil::unique_prop_variant propVariantAppName;
    wil::unique_cotaskmem_string prodName = wil::make_unique_string<wil::unique_cotaskmem_string>(L"CppUnpackagedAppNotifications");
    propVariantAppName.pwszVal = prodName.release();
    propVariantAppName.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchDisplayNameResource, propVariantAppName));
}
CATCH_LOG()

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    App::App()
    {
        winrt::ActivationRegistrationManager::RegisterForStartupActivation(
            L"ToastSampleAppId",
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

        SetDisplayNameAndIcon();

        auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
        const auto token = notificationManager.NotificationInvoked([&](const auto&, const winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs& notificationActivatedEventArgs)
            {
                std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };

                if (args.find(L"activateToast") != std::wstring::npos)
                {
                    MainPage::Current().NotifyUser(L"Successful activation from toast!", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                }

                if (args.find(L"reply") != std::wstring::npos)
                {
                    auto input{ notificationActivatedEventArgs.UserInput() };
                    auto text{ input.Lookup(L"tbReply") };

                    std::wstring message{ L"Successful activation from toast! [" };
                    message.append(text);
                    message.append(L"]");

                    MainPage::Current().NotifyUser(message.c_str(), Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                }
#if 0
                // Check if activated from background by AppNotification
                if (args. == winrt::ExtendedActivationKind::AppNotification)
                {
                }
#endif

                //window.Activate();
                //window.Content().
                //MainPage::Current().
                  //rootPage.NotifyUser(L"Toast Activation Received!", InfoBarSeverity::Informational);
                    //ProcessNotificationArgs(notificationActivatedEventArgs);
                });

        notificationManager.Register();

        window.Activate();
    }

    void App::OnActivated(winrt::IActivatedEventArgs const&)
    {
        window = winrt::make<MainWindow>();

        SetDisplayNameAndIcon();

        auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
        const auto token = notificationManager.NotificationInvoked([&](const auto&, const winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs& notificationActivatedEventArgs)
            {
                std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };

                if (args.find(L"activateToast") != std::wstring::npos)
                {
                    MainPage::Current().NotifyUser(L"Successful activation from toast!", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                }

                if (args.find(L"reply") != std::wstring::npos)
                {
                    auto input{ notificationActivatedEventArgs.UserInput() };
                    auto text{ input.Lookup(L"tbReply") };

                    std::wstring message{ L"Successful activation from toast! [" };
                    message.append(text);
                    message.append(L"]");

                    MainPage::Current().NotifyUser(message.c_str(), Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                }
#if 0
                // Check if activated from background by AppNotification
                if (args. == winrt::ExtendedActivationKind::AppNotification)
                {
                }
#endif
                //window.Activate();
                //window.Content().
                //MainPage::Current().
                  //rootPage.NotifyUser(L"Toast Activation Received!", InfoBarSeverity::Informational);
                    //ProcessNotificationArgs(notificationActivatedEventArgs);
            });

        notificationManager.Register();

        window.Activate();
    }

}
