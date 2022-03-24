// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <wil/result.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
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
namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Windows::ApplicationModel::Activation;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
    using namespace winrt::Windows::Storage;
}



// This function is intended to be called in the unpackaged scenario.
void SetDisplayNameAndIcon(HWND hwnd) noexcept try
{
    // Not mandatory, but it's highly recommended to specify AppUserModelId
    //THROW_IF_FAILED(SetCurrentProcessExplicitAppUserModelID(L"ToastSampleAppId"));

    // Icon is mandatory
    winrt::com_ptr<IPropertyStore> propertyStore;
    //wil::unique_hwnd hWindow{ GetConsoleWindow()};

    THROW_IF_FAILED(SHGetPropertyStoreForWindow(hwnd /*hWindow.get()*/, IID_PPV_ARGS(&propertyStore)));

    wil::unique_prop_variant propVariantIcon;
    wil::unique_cotaskmem_string sth = wil::make_unique_string<wil::unique_cotaskmem_string>(LR"("Assets\Square44x44Logo.png")" /*LR"(%SystemRoot%\system32\@WLOGO_96x96.png)"*/);
    propVariantIcon.pwszVal = sth.release();
    propVariantIcon.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchIconResource, propVariantIcon));
#if 0
    // App name is not mandatory, but it's highly recommended to specify it
    wil::unique_prop_variant propVariantAppName;
    wil::unique_cotaskmem_string prodName = wil::make_unique_string<wil::unique_cotaskmem_string>(L"CppPackagedAppNotifications4");
    propVariantAppName.pwszVal = prodName.release();
    propVariantAppName.vt = VT_LPWSTR;
    THROW_IF_FAILED(propertyStore->SetValue(PKEY_AppUserModel_RelaunchDisplayNameResource, propVariantAppName));
#endif
}
CATCH_LOG()
namespace winrt::CppPackagedAppNotifications4::implementation
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
        // NOTE: OnLaunched will always report that the ActivationKind == Launch,
        // even when it isn't.
        winrt::Windows::ApplicationModel::Activation::ActivationKind kind
            = args.UWPLaunchActivatedEventArgs().Kind();
        OutputFormattedMessage(L"OnLaunched: Kind=%s", KindString(kind).c_str());

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
                OutputFormattedMessage(L"activationArgs.Kind=%s", KindString(extendedKind).c_str());
            }
        }

        window = make<MainWindow>();

        HWND hwnd;
        window.try_as<IWindowNative>()->get_WindowHandle(&hwnd);
		SetDisplayNameAndIcon(hwnd);

        auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
        const auto token = notificationManager.NotificationInvoked([&](const auto&, const winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs& notificationActivatedEventArgs)
            {
                std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };
                if (args.find(L"activateToast") != std::wstring::npos)
                {
                    MainPage::Current().NotifyUser(L"NotificationInvoked: Successful invocation from toast!", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
                }

                if (args.find(L"reply") != std::wstring::npos)
                {
                    auto input{ notificationActivatedEventArgs.UserInput() };
                    auto text{ input.Lookup(L"tbReply") };

                    std::wstring message{ L"NotificationInvoked: Successful invocation from toast! [" };
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

    // Helpers ////////////////////////////////////////////////////////////////////
    int activationCount = 1;
    //event_token activationToken;
    winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> messages = winrt::single_threaded_observable_vector<winrt::Windows::Foundation::IInspectable>();

    void OutputMessage(const WCHAR* message)
    {
        messages.Append(winrt::PropertyValue::CreateString(message));
    }

    void OutputFormattedMessage(const WCHAR* fmt, ...)
    {
        WCHAR message[1025];
        va_list args;
        va_start(args, fmt);
        wvsprintf(message, fmt, args);
        va_end(args);
        OutputMessage(message);
    }

    std::vector<std::wstring> SplitStrings(winrt::hstring argString)
    {
        std::vector<std::wstring> argStrings;
        std::wistringstream iss(argString.c_str());
        for (std::wstring s; iss >> s; )
        {
            argStrings.push_back(s);
        }
        return argStrings;
    }

    ///////////////////////////////////////////////////////////////////////////////


    // Rich activation ////////////////////////////////////////////////////////////

    void GetActivationInfo()
    {
        winrt::AppActivationArguments args = winrt::AppInstance::GetCurrent().GetActivatedEventArgs();
        winrt::ExtendedActivationKind kind = args.Kind();
        if (kind == winrt::ExtendedActivationKind::Launch)
        {
            auto launchArgs = args.Data().as<
                winrt::Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs>();
            if (launchArgs)
            {
                auto argString = launchArgs.Arguments();
                std::vector<std::wstring> argStrings = SplitStrings(argString);
                OutputMessage(L"Launch activation");
                for (std::wstring const& s : argStrings)
                {
                    OutputMessage(s.c_str());
                }
            }
        }
        else if (kind == winrt::ExtendedActivationKind::File)
        {
            auto fileArgs = args.Data().as<winrt::IFileActivatedEventArgs>();
            if (fileArgs)
            {
                winrt::IStorageItem file = fileArgs.Files().GetAt(0);
                OutputFormattedMessage(
                    L"File activation for '%s'", file.Name().c_str());
            }
        }
    }

    void ReportLaunchArgs(winrt::hstring callLocation, winrt::AppActivationArguments args)
    {
        winrt::Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs launchArgs =
            args.Data().as<winrt::Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            OutputFormattedMessage(L"Launch activation (%s)", callLocation.c_str());
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }

    void ReportFileArgs(winrt::hstring callLocation, winrt::AppActivationArguments args)
    {
        winrt::IFileActivatedEventArgs fileArgs = args.Data().as<winrt::IFileActivatedEventArgs>();
        if (fileArgs)
        {
            winrt::IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation (%s) for '%s'", callLocation.c_str(), file.Name().c_str());
        }
    }
