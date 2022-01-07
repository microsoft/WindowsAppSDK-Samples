// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.ApplicationModel.Resources;

namespace SelfContainedDeployment
{
    public partial class Scenario1_SelfContainedDeployment : Page
    {
        private MainPage rootPage = MainPage.Current;

        private ResourceManager resourceManager = new ResourceManager();

        public Scenario1_SelfContainedDeployment()
        {
            this.InitializeComponent();

            Description.Text = resourceManager.MainResourceMap.GetValue("Resources/Description").ValueAsString;
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
