// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "S2_CrashRecovery.g.h"

namespace winrt::cpp_winui_packaged::implementation
{
    struct S2_CrashRecovery : S2_CrashRecoveryT<S2_CrashRecovery>
    {
        S2_CrashRecovery();
        void Crash_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs e);

    private:
        void SetRecoveredCounter();
        void StartTimer();
        void Timer_OnTick(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);

        winrt::Microsoft::UI::Xaml::DispatcherTimer _timer;
        uint32_t _counter;
    };
}

namespace winrt::cpp_winui_packaged::factory_implementation
{
    struct S2_CrashRecovery : S2_CrashRecoveryT<S2_CrashRecovery, implementation::S2_CrashRecovery>
    {
    };
}
