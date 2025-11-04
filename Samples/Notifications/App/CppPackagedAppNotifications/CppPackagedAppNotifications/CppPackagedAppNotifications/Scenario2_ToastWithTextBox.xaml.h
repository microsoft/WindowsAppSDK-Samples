// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_ToastWithTextBox.g.h"

namespace winrt::CppPackagedAppNotifications::implementation
{
    struct Scenario2_ToastWithTextBox : Scenario2_ToastWithTextBoxT<Scenario2_ToastWithTextBox>
    {
        Scenario2_ToastWithTextBox();

        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppPackagedAppNotifications::MainPage rootPage;
    };
}

namespace winrt::CppPackagedAppNotifications::factory_implementation
{
    struct Scenario2_ToastWithTextBox : Scenario2_ToastWithTextBoxT<Scenario2_ToastWithTextBox, implementation::Scenario2_ToastWithTextBox>
    {
    };
}
