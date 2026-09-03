// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <stdlib.h>
#include "S2_CrashRecovery.xaml.h"
#if __has_include("S2_CrashRecovery.g.cpp")
#include "S2_CrashRecovery.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Windows::Foundation;
}

namespace winrt::cpp_winui_packaged::implementation
{
    S2_CrashRecovery::S2_CrashRecovery()
    {
        InitializeComponent();
        SetRecoveredCounter();

        // Best effort to register for crash and hang restart.
        RegisterApplicationRestart(counterTextBlock().Text().c_str(), RESTART_NO_PATCH | RESTART_NO_REBOOT);

        // Best effort to register recovery callback to update restart arguments with the latest seconds counter value.
        RegisterApplicationRecoveryCallback(APPLICATION_RECOVERY_CALLBACK([](PVOID recoveryData){
            BOOL canceled;
            if (SUCCEEDED(ApplicationRecoveryInProgress(&canceled)) && !canceled)
            {
                const auto counter = static_cast<uint32_t*>(recoveryData);
                RegisterApplicationRestart(to_hstring(*counter).c_str(), RESTART_NO_PATCH | RESTART_NO_REBOOT);
            }

            ApplicationRecoveryFinished(true);
            return (DWORD)0;
            }), &_counter, RECOVERY_DEFAULT_PING_INTERVAL, 0);

        StartTimer();
    }

    void S2_CrashRecovery::SetRecoveredCounter()
    {
        winrt::hstring cmdLineString = GetCommandLineW();
        int numArgs;
        const auto cmdLineArgs = CommandLineToArgvW(cmdLineString.c_str(), &numArgs);
        if (numArgs > 1)
        {
            auto argString(to_string(cmdLineArgs[1]));
            _counter = strtoul(argString.c_str(), nullptr, 10);
        }
        else
        {
            _counter = 0;
        }

        counterTextBlock().Text(to_hstring(_counter));
    }

    void S2_CrashRecovery::StartTimer()
    {
        _timer = DispatcherTimer();
        _timer.Tick({ this, &S2_CrashRecovery::Timer_OnTick });
        _timer.Interval(std::chrono::seconds(1));
        _timer.Start();
    }

    void S2_CrashRecovery::Timer_OnTick(IInspectable const&, IInspectable const&)
    {
        _counter++;
        counterTextBlock().Text(to_hstring(_counter));
    }

    void S2_CrashRecovery::Crash_Click(IInspectable const&, RoutedEventArgs const&)
    {
        for (int i = 60; i >= 0; i--)
        {
            Sleep(1000);
        }

        // crash the application by writing to a null pointer.
        *((int*)nullptr) = 0;
    }

    void S2_CrashRecovery::OnNavigatedFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs e)
    {
        _timer.Stop();
    }
}
