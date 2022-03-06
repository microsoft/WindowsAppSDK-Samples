// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ShortName.g.h"

namespace winrt::TestCppWinUiUnpackaged::implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName>
    {
        Scenario1_ShortName();
        void DisplayStatus_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static TestCppWinUiUnpackaged::MainPage rootPage;
    };
}

namespace winrt::TestCppWinUiUnpackaged::factory_implementation
{
    struct Scenario1_ShortName : Scenario1_ShortNameT<Scenario1_ShortName, implementation::Scenario1_ShortName>
    {
    };
}
