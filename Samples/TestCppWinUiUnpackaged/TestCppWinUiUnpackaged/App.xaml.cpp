// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <MddBootstrap.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace winrt::Windows::ApplicationModel::Activation;
    using namespace winrt::Microsoft::Windows::AppLifecycle;
}

HRESULT LoadWinAppSDK()
{
    const UINT32 majorMinorVersion{ 0x00010000 };
    PCWSTR versionTag{ L"" };
    const PACKAGE_VERSION minVersion{};
    const HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };

    // Check the return code for errors. If there is an error, display the result.
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    return S_OK;
}

namespace winrt::TestCppWinUiUnpackaged::implementation
{
    App::App()
    {
        if (FAILED(LoadWinAppSDK()))
        {
            // Handle failure here...
        }

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
    }
}
