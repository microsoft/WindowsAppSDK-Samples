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
        void activityBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void alarmBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void alertBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void attentionBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void availableBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void awayBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void busyBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void errorBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void newMessageBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void pausedBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void playingBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void unavailableBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void clearBadge_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        void showBadge(winrt::Microsoft::Windows::BadgeNotifications::BadgeNotificationGlyph glyphValue);
        static cpp_winui::MainPage rootPage;
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct Scenario2_GlyphBadge : Scenario2_GlyphBadgeT<Scenario2_GlyphBadge, implementation::Scenario2_GlyphBadge>
    {
    };
}
