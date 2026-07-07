// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario3_PushToastWithAvatar.g.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    struct Scenario3_PushToastWithAvatar : Scenario3_PushToastWithAvatarT<Scenario3_PushToastWithAvatar>
    {
        Scenario3_PushToastWithAvatar();

        void SendToast_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        static CppUnpackagedAppNotifications::MainPage rootPage;
    };
}

namespace winrt::CppUnpackagedAppNotifications::factory_implementation
{
    struct Scenario3_PushToastWithAvatar : Scenario3_PushToastWithAvatarT<Scenario3_PushToastWithAvatar, implementation::Scenario3_PushToastWithAvatar>
    {
    };
}
