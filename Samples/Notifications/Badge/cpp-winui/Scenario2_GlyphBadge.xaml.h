// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_GlyphBadge.g.h"
#include <winrt/Microsoft.Windows.BadgeNotifications.h>

namespace winrt::cpp_winui::implementation
{
    struct Scenario2_GlyphBadge : Scenario2_GlyphBadgeT<Scenario2_GlyphBadge>
    {
        Scenario2_GlyphBadge();
        void badgeButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        static cpp_winui::MainPage rootPage;
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct Scenario2_GlyphBadge : Scenario2_GlyphBadgeT<Scenario2_GlyphBadge, implementation::Scenario2_GlyphBadge>
    {
    };
}
