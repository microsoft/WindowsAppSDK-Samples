// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "NotificationManager.h"
#include <wil/result.h>
#include <Microsoft.UI.Xaml.Window.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <propkey.h> //PKEY properties
#include <ShObjIdl_core.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Microsoft::Windows::AppNotifications;
}

static NotificationManager g_notificationManager;

// This function is intended to be called in the unpackaged scenario.
void SetDisplayNameAndIcon(HWND hwnd) noexcept try
{
    // Not mandatory, but it's highly recommended to specify AppUserModelId
    THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"ba5623de-515a-4d7e-9110-38d1641f5fe0"));

    // Icon is mandatory
    winrt::com_ptr<IPropertyStore> propertyStore;
    //wil::unique_hwnd hWindow{ GetConsoleWindow() };

    THROW_IF_FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&propertyStore)));

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

        HWND hwnd;
        window.try_as<IWindowNative>()->get_WindowHandle(&hwnd);
        SetDisplayNameAndIcon(hwnd);

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
