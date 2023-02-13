// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "S1_UpdateReboot.g.h"

namespace winrt::cpp_winui_packaged::implementation
{
    struct S1_UpdateReboot : S1_UpdateRebootT<S1_UpdateReboot>
    {
        S1_UpdateReboot();
        void Message_TextChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&);
        void Register_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Unregister_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        void SetRecoveredMassage();
    };
}

namespace winrt::cpp_winui_packaged::factory_implementation
{
    struct S1_UpdateReboot : S1_UpdateRebootT<S1_UpdateReboot, implementation::S1_UpdateReboot>
    {
    };
}
