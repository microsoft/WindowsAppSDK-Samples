// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include "S3_RebootTypeDetection.xaml.h"
#if __has_include("S3_RebootTypeDetection.g.cpp")
#include "S3_RebootTypeDetection.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::cpp_winui_packaged::implementation
{
    S3_RebootTypeDetection::S3_RebootTypeDetection()
    {
        InitializeComponent();
        SetRestartTypeText();

        // Best effort to register for crash and hang restart.
        RegisterApplicationRestart(L"normal", 0);

        // Best effort to register recovery callback to update restart arguments with the latest seconds counter value.
        RegisterApplicationRecoveryCallback(APPLICATION_RECOVERY_CALLBACK([](PVOID) {
            BOOL canceled;
            if (SUCCEEDED(ApplicationRecoveryInProgress(&canceled)) && !canceled)
            {
                RegisterApplicationRestart(L"abnormal", RESTART_NO_PATCH | RESTART_NO_REBOOT);
            }

            ApplicationRecoveryFinished(true);
            return (DWORD)0;
            }), nullptr, RECOVERY_DEFAULT_PING_INTERVAL, 0);
    }

    void S3_RebootTypeDetection::SetRestartTypeText()
    {
        winrt::hstring cmdLineString = GetCommandLineW();
        int numArgs;
        const auto cmdLineArgs = CommandLineToArgvW(cmdLineString.c_str(), &numArgs);
        if (numArgs > 1)
        {
            restartTypeTextBlock().Text(std::wstring(L"App recovered from " + to_hstring(cmdLineArgs[1]) + L" restart"));
        }
    }

    void S3_RebootTypeDetection::Crash_Click(IInspectable const&, RoutedEventArgs const&)
    {
        for (int i = 60; i >= 0; i--)
        {
            Sleep(1000);
        }

        // crash the application by writing to a null pointer.
        *((int*)nullptr) = 0;
    }

    void S3_RebootTypeDetection::Restart_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::Microsoft::Windows::AppLifecycle::AppInstance::Restart(to_hstring("normal"));
    }
}
