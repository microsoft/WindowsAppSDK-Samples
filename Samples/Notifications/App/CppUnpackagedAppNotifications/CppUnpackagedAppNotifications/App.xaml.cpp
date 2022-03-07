// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <MddBootstrap.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::AppNotifications;
}

HRESULT LoadWindowsAppSDK()
{
#if 0
    // Major.Minor version, MinVersion=0 to find any framework package for this major.minor version
    const UINT32 majorMinorVersion{ 0x00010001 }; //  { Major << 16) | Minor };
    PCWSTR versionTag{ L"" };
    const PACKAGE_VERSION minVersion{};
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"\nError 0x%08X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }
#endif
    return S_OK;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    App::App()
    {
        if (SUCCEEDED(::LoadWindowsAppSDK()))
        {
            winrt::AppNotificationManager::Default().Register();
            //infoBar().Message(L"Could not load WindowsAppSDK", InfoBarSeverity::Error);
        }

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

    void App::OnLaunched(winrt::LaunchActivatedEventArgs const&)
    {
        window = winrt::make<MainWindow>();
        window.Activate();
    }
}
