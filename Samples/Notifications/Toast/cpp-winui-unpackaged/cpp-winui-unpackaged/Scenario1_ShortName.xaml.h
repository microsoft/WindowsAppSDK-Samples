// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ShortName.g.h"

namespace winrt::cpp_winui_unpackaged::implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName>
    {
        Scenario1_ShortName();
        void SuccessMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ErrorMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void InformationalMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static cpp_winui_unpackaged::MainPage rootPage;
    };
}

namespace winrt::cpp_winui_unpackaged::factory_implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName, implementation::Scenario1_ShortName>
    {
    };
}
