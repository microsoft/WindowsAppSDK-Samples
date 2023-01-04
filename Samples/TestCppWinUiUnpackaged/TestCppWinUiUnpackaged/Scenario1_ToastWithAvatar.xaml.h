// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ToastWithAvatar.g.h"

namespace winrt::TestCppWinUiUnpackaged::implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar>
    {
        Scenario1_ToastWithAvatar();
        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static TestCppWinUiUnpackaged::MainPage rootPage;
    };
}

namespace winrt::TestCppWinUiUnpackaged::factory_implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar, implementation::Scenario1_ToastWithAvatar>
    {
    };
}
