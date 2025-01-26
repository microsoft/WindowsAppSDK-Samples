// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario2_GlyphBadge.xaml.h"
#if __has_include("Scenario2_GlyphBadge.g.cpp")
#include "Scenario2_GlyphBadge.g.cpp"
#include <winrt/Microsoft.Windows.BadgeNotifications.h>
#endif

using namespace winrt;

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::BadgeNotifications;
}

namespace winrt::cpp_winui::implementation
{
    MainPage Scenario2_GlyphBadge::rootPage{ nullptr };

    Scenario2_GlyphBadge::Scenario2_GlyphBadge()
    {
        InitializeComponent();
        Scenario2_GlyphBadge::rootPage = MainPage::Current();
    }

    void Scenario2_GlyphBadge::showBadge(winrt::BadgeNotificationGlyph glyphValue) {
        try {
            winrt::BadgeNotificationManager::Current().SetBadgeAsGlyph(glyphValue);
            rootPage.NotifyUser(L"Badge Notification Displayed Successfully.", InfoBarSeverity::Informational);
        }
        catch (winrt::hresult_error const& ex) {
            rootPage.NotifyUser(L"An error occurred: " + ex.message(), InfoBarSeverity::Informational);
        }
    }

    void Scenario2_GlyphBadge::activityBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Activity);
    }

    void Scenario2_GlyphBadge::alarmBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Alarm);
    }

    void Scenario2_GlyphBadge::alertBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Alert);
    }

    void Scenario2_GlyphBadge::attentionBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Attention);
    }

    void Scenario2_GlyphBadge::availableBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Available);
    }

    void Scenario2_GlyphBadge::awayBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Away);
    }

    void Scenario2_GlyphBadge::busyBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Busy);
    }

    void Scenario2_GlyphBadge::errorBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Error);
    }

    void Scenario2_GlyphBadge::newMessageBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::NewMessage);
    }

    void Scenario2_GlyphBadge::pausedBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Paused);
    }

    void Scenario2_GlyphBadge::playingBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Playing);
    }

    void Scenario2_GlyphBadge::unavailableBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        showBadge(winrt::BadgeNotificationGlyph::Unavailable);
    }

    void Scenario2_GlyphBadge::clearBadge_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        winrt::BadgeNotificationManager::Current().ClearBadge();
        rootPage.NotifyUser(L"Badge Notification Cleared Successfully.", InfoBarSeverity::Informational);
    }
}
