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

    void Scenario2_GlyphBadge::badgeButton_Click(IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) {
        auto button = sender.as<Button>();
        auto tag = button.Tag().as<hstring>();

        if (tag == L"Clear") {
            winrt::BadgeNotificationManager::Current().ClearBadge();
            rootPage.NotifyUser(L"Badge Notification Cleared Successfully.", InfoBarSeverity::Informational);
            return;
        }

        static const std::map<hstring, winrt::BadgeNotificationGlyph> badgeMap = {
            { L"Activity", winrt::BadgeNotificationGlyph::Activity },
            { L"Alarm", winrt::BadgeNotificationGlyph::Alarm },
            { L"Alert", winrt::BadgeNotificationGlyph::Alert },
            { L"Attention", winrt::BadgeNotificationGlyph::Attention },
            { L"Available", winrt::BadgeNotificationGlyph::Available },
            { L"Away", winrt::BadgeNotificationGlyph::Away },
            { L"Busy", winrt::BadgeNotificationGlyph::Busy },
            { L"Error", winrt::BadgeNotificationGlyph::Error },
            { L"NewMessage", winrt::BadgeNotificationGlyph::NewMessage },
            { L"Paused", winrt::BadgeNotificationGlyph::Paused },
            { L"Playing", winrt::BadgeNotificationGlyph::Playing },
            { L"Unavailable", winrt::BadgeNotificationGlyph::Unavailable }
        };

        auto it = badgeMap.find(tag);
        if (it != badgeMap.end()) {
            winrt::BadgeNotificationManager::Current().SetBadgeAsGlyph(it->second);
            rootPage.NotifyUser(L"Badge Notification Displayed Successfully.", InfoBarSeverity::Informational);
        }

        return;
    }
}
