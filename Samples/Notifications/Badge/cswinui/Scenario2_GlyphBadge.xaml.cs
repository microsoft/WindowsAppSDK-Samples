// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.BadgeNotifications;
using System;

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

        private void ShowBadge(BadgeNotificationGlyph glyphValue)
        {
            try
            {
                BadgeNotificationManager.Current.SetBadgeAsGlyph(glyphValue);
                rootPage.NotifyUser("Badge Notification Displayed Successfully.", InfoBarSeverity.Informational);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("An error occurred: " + ex.Message, InfoBarSeverity.Informational);
            }
        }

        private void ActivityBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Activity);
        }

        private void AlarmBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Alarm);
        }

        private void AlertBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Alert);
        }

        private void AttentionBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Attention);
        }

        private void AvailableBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Available);
        }

        private void AwayBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Away);
        }

        private void BusyBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Busy);
        }

        private void ErrorBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Error);
        }

        private void NewMessageBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.NewMessage);
        }

        private void PausedBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Paused);
        }

        private void PlayingBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Playing);
        }

        private void UnavailableBadge_Click(object sender, RoutedEventArgs e)
        {
            ShowBadge(BadgeNotificationGlyph.Unavailable);
        }

        private void ClearBadge_Click(object sender, RoutedEventArgs e)
        {
            BadgeNotificationManager.Current.ClearBadge();
            rootPage.NotifyUser("Badge Notification Cleared Successfully.", InfoBarSeverity.Informational);
        }
    }
}
