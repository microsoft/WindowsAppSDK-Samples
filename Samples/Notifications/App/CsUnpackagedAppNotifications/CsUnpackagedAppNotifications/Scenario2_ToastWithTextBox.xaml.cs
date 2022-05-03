﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace CsUnpackagedAppNotifications
{
    public partial class Scenario2_ToastWithTextBox : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_ToastWithTextBox()
        {
            this.InitializeComponent();
        }

        private void SendToast_Click(object sender, RoutedEventArgs e)
        {
            if (ToastWithTextBox.SendToast())
            {
                rootPage.NotifyUser("Toast sent successfully!", InfoBarSeverity.Success);
            }
            else
            {
                rootPage.NotifyUser("Could not send toast", InfoBarSeverity.Error);
            }
        }
    }
}
