// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.BadgeNotifications;
using System;
using System.Linq;

namespace cswinui
{
    public partial class Scenario1_NumericBadge : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_NumericBadge()
        {
            this.InitializeComponent();
        }

        private void ShowNumericBadge_Click(object sender, RoutedEventArgs e)
        {
            string inputText = inputTextBox.Text;
            if (string.IsNullOrEmpty(inputText))
            {
                rootPage.NotifyUser("Please enter a number.", InfoBarSeverity.Informational);
                return;
            }

            if (!inputText.All(char.IsDigit))
            {
                rootPage.NotifyUser("Please enter a valid number.", InfoBarSeverity.Informational);
                return;
            }

            uint badgeCount = uint.Parse(inputText);

            if (badgeCount < 0)
            {
                rootPage.NotifyUser("Please enter a positive number.", InfoBarSeverity.Informational);
                return;
            }

            try
            {
                BadgeNotificationManager.Current.SetBadgeAsCount(badgeCount);
                rootPage.NotifyUser("Badge Notification Displayed Successfully.", InfoBarSeverity.Informational);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser($"An error occurred: {ex.Message}", InfoBarSeverity.Informational);
            }
        }
    }
}
