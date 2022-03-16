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
    THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"ToastSampleAppId")); // elx - need to get a proper app id

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
    wil::unique_cotaskmem_string prodName = wil::make_unique_string<wil::unique_cotaskmem_string>(L"The Toast Notification Sample");
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

        SetDisplayNameAndIcon();
        winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Register();

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
        auto activatedEventArgs{ winrt::AppInstance::GetCurrent().GetActivatedEventArgs() };

        // Check if activated from background by AppNotification
        if (activatedEventArgs.Kind() == winrt::ExtendedActivationKind::AppNotification)
        {
            winrt::Controls::ContentDialog dialog;
            dialog.Title(box_value(L"title"));
            dialog.Content(box_value(L"content"));
            dialog.PrimaryButtonText(L"primary");
            dialog.CloseButtonText(L"close");

            auto result = dialog.ShowAsync();
            //dialog.Title(L"Hello MessageDialog");
            //dialog.XamlRoot(Content().XamlRoot() /* Assuming that you're showing from the window */);

#if 0
            winrt::Windows::UI::Popups::MessageDialog dialog1("No internet connection has been found.");

            winrt::Windows::UI::Popups::MessageDialog dialog2(L"Hello MessageDialog");
            dialog2.ShowAsync();
#endif
#if 0
            ContentDialog dialog;
            dialog.Title(box_value(L"title"));
            dialog.Content(box_value(L"content"));
            dialog.PrimaryButtonText(L"primary");
            dialog.CloseButtonText(L"close");
            //dialog.XamlRoot(myButton().XamlRoot()); // maybe needed, maybe not

            auto result = co_await dialog.ShowAsync();

            if (result == ContentDialogResult::Primary)
            {

            }
#endif
#if 0
            std::wcout << L"Activated by AppNotification from background.\n";
            winrt::AppNotificationActivatedEventArgs appNotificationArgs{ args.Data().as<winrt::AppNotificationActivatedEventArgs>() };
            winrt::hstring arguments{ appNotificationArgs.Argument() };
            std::wcout << arguments.c_str() << std::endl << std::endl;

            winrt::IMap<winrt::hstring, winrt::hstring> userInput = appNotificationArgs.UserInput();
            for (auto pair : userInput)
            {
                std::wcout << "Key= " << pair.Key().c_str() << " " << "Value= " << pair.Value().c_str() << L"\n";
            }
            std::wcout << std::endl;
#endif
        }

        window = winrt::make<MainWindow>();
        window.Activate();
    }
}
