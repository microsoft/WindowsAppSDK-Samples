// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace $safeprojectname$
{
    public partial class Scenario1_ShortName : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_ShortName()
        {
            this.InitializeComponent();
        }

        private void SuccessMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Everything was ok!", InfoBarSeverity.Success);
        }

        private void ErrorMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Something went wrong.", InfoBarSeverity.Error);
        }

        private void InformationalMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("This is the informational bar.", InfoBarSeverity.Informational);
        }

        private void ClearMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", InfoBarSeverity.Informational, false);
        }
    }
}
