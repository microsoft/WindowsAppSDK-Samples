// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_ToastWithTextBox.g.h"

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    struct Scenario2_ToastWithTextBox : Scenario2_ToastWithTextBoxT<Scenario2_ToastWithTextBox>
    {
        Scenario2_ToastWithTextBox();

        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppUnpackagedAppNotifications::MainPage rootPage;
    };
}

namespace winrt::CppUnpackagedAppNotifications::factory_implementation
{
    struct Scenario2_ToastWithTextBox : Scenario2_ToastWithTextBoxT<Scenario2_ToastWithTextBox, implementation::Scenario2_ToastWithTextBox>
    {
    };
}
