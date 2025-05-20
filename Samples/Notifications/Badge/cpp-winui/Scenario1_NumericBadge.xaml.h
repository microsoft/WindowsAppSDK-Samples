// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_NumericBadge.g.h"

namespace winrt::cpp_winui::implementation
{
    struct Scenario1_NumericBadge : Scenario1_NumericBadgeT<Scenario1_NumericBadge>
    {
        Scenario1_NumericBadge();
        void showNumericBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    private:
        static cpp_winui::MainPage rootPage;
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct Scenario1_NumericBadge : Scenario1_NumericBadgeT<Scenario1_NumericBadge, implementation::Scenario1_NumericBadge>
    {
    };
}
