﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_ToastWithAvatar.g.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>

namespace winrt::CppPackagedAppNotifications::implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar>
    {
        Scenario1_ToastWithAvatar();

        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppPackagedAppNotifications::MainPage rootPage;
    };
}

namespace winrt::CppPackagedAppNotifications::factory_implementation
{
    struct Scenario1_ToastWithAvatar : Scenario1_ToastWithAvatarT<Scenario1_ToastWithAvatar, implementation::Scenario1_ToastWithAvatar>
    {
    };
}
