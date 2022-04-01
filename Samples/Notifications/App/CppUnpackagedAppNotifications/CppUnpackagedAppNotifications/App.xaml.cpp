﻿// Copyright (c) Microsoft Corporation.
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

#include <propkey.h> //PKEY properties
#include <propsys.h>
#include <ShObjIdl_core.h>

#include <sstream>
#include <winrt/Windows.Storage.h>
#include <Microsoft.UI.Xaml.Window.h>
#include "Utils.h"

#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Windows::ApplicationModel::Activation;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Windows::Storage;
    using namespace winrt::Microsoft::Windows::AppNotifications;
}

// This function is intended to be called in the unpackaged scenario.
void SetDisplayNameAndIcon(HWND hwnd) noexcept try
{
    // Not mandatory, but it's highly recommended to specify AppUserModelId
    THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"ba5623de-515a-4d7e-9110-38d1641f5fe0"));

    // Icon is mandatory
    winrt::com_ptr<IPropertyStore> propertyStore;
    //wil::unique_hwnd hWindow{ GetConsoleWindow() };

    THROW_IF_FAILED(SHGetPropertyStoreForWindow(hwnd /*hWindow.get()*/, IID_PPV_ARGS(&propertyStore)));

    wil::unique_prop_variant propVariantIcon;
    wil::unique_cotaskmem_string sth = wil::make_unique_string<wil::unique_cotaskmem_string>(LR"("Assets\Square44x44Logo.png")" /*LR"(%SystemRoot%\system32\@WLOGO_96x96.png)"*/);
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

    // Enum-to-string helpers. This app only supports Launch and File activation.
    // Note that ExtendedActivationKind is a superset of ActivationKind, so 
    // we could reduce these 2 methods to one, and cast appropriately from
    // ActivationKind to ExtendedActivationKind. However, this sample keeps
    // them separate to illustrate the difference between Xaml::LaunchActivatedEventArgs
    // and AppLifecycle::AppActivationArguments
    winrt::hstring KindString(
        winrt::Windows::ApplicationModel::Activation::ActivationKind kind)
    {
        using namespace winrt::Windows::ApplicationModel::Activation;
        switch (kind)
        {
        case ActivationKind::Launch: return winrt::hstring(L"Launch");
        case ActivationKind::File: return winrt::hstring(L"File");
        default: return winrt::hstring(L"Unknown");
        }
    }

    winrt::hstring KindString(
        winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind extendedKind)
    {
        using namespace winrt::Microsoft::Windows::AppLifecycle;
        switch (extendedKind)
        {
        case ExtendedActivationKind::Launch: return winrt::hstring(L"Launch");
        case ExtendedActivationKind::File: return winrt::hstring(L"File");
        default: return winrt::hstring(L"Unknown");
        }
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& args)
    {
        window = make<MainWindow>();

        HWND hwnd;
        window.try_as<IWindowNative>()->get_WindowHandle(&hwnd);
        SetDisplayNameAndIcon(hwnd);

        auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
        const auto token = notificationManager.NotificationInvoked([&](const auto&, winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
            {
                if (!Utils::DispatchNotification(notificationActivatedEventArgs))
                {
                    MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Error);
                }
                //MainPage::Current().NotificationDialog();
            });

        notificationManager.Register();
#if 0
        //try
        {
            // NOTE: AppInstance is ambiguous between
            // Microsoft.Windows.AppLifecycle.AppInstance and
            // Windows.ApplicationModel.AppInstance
            auto currentInstance = winrt::Microsoft::Windows::AppLifecycle::AppInstance::GetCurrent();
            if (currentInstance)
            {
                // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
                // even in WinUI's OnLaunched.
                winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments activationArgs
                    = currentInstance.GetActivatedEventArgs();
                if (activationArgs)
                {
                    winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind extendedKind
                        = activationArgs.Kind();
                    if (extendedKind == winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind::AppNotification)
                    {
                        //UpdateStatus(L"AppNotification", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                        winrt::AppNotificationActivatedEventArgs notificationActivatedEventArgs = activationArgs.Data().as<winrt::AppNotificationActivatedEventArgs>();

                        std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };
                        if (args.find(L"activateToast") != std::wstring::npos)
                        {
                            ToastWithAvatar::NotificationReceived(notificationActivatedEventArgs);
                            MainPage::Current().NotifyUser(L"App Notification Instanciation", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                        }
                        else if (args.find(L"reply") != std::wstring::npos)
                        {
                            ToastWithTextBox::NotificationReceived(notificationActivatedEventArgs);
                            MainPage::Current().NotifyUser(L"App Notification Instanciation", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
    }
                        else
                        {
                            MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Error);
                        }
}
                    else
                    {
                        MainPage::Current().NotifyUser(L"Normal launch", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                    }
                }
            }
        }
        //catch (...)
        //{
            // toast activation
        //}
#endif
        window.Activate();
    }
}
