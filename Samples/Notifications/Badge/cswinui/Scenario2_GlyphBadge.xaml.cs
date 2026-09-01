// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.BadgeNotifications;
using System;
using System.Collections.Generic;

namespace cswinui
{
    public sealed partial class Scenario2_GlyphBadge : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_GlyphBadge()
        {
            this.InitializeComponent();
            rootPage = MainPage.Current;
        }

        private void badgeButton_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            var tag = button.Tag as string;

            if (tag == "Clear")
            {
                BadgeNotificationManager.Current.ClearBadge();
                rootPage.NotifyUser("Badge Notification Cleared Successfully.", InfoBarSeverity.Informational);
                return;
            }

            var badgeMap = new Dictionary<string, BadgeNotificationGlyph>
            {
                { "Activity", BadgeNotificationGlyph.Activity },
                { "Alarm", BadgeNotificationGlyph.Alarm },
                { "Alert", BadgeNotificationGlyph.Alert },
                { "Attention", BadgeNotificationGlyph.Attention },
                { "Available", BadgeNotificationGlyph.Available },
                { "Away", BadgeNotificationGlyph.Away },
                { "Busy", BadgeNotificationGlyph.Busy },
                { "Error", BadgeNotificationGlyph.Error },
                { "NewMessage", BadgeNotificationGlyph.NewMessage },
                { "Paused", BadgeNotificationGlyph.Paused },
                { "Playing", BadgeNotificationGlyph.Playing },
                { "Unavailable", BadgeNotificationGlyph.Unavailable }
            };

            if (badgeMap.TryGetValue(tag, out BadgeNotificationGlyph glyph))
            {
                BadgeNotificationManager.Current.SetBadgeAsGlyph(glyph);
                rootPage.NotifyUser("Badge Notification Displayed Successfully.", InfoBarSeverity.Informational);
            }

            return;
        }
    }
}
