// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System.Linq;

namespace SelfContainedDeployment
{
    public partial class SettingsPage : Page
    {
        public SettingsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            themePanel.Children.Cast<RadioButton>().First(c => (ElementTheme)c.Tag == SampleConfig.CurrentTheme).IsChecked = true;
            base.OnNavigatedTo(e);
        }

        private void OnThemeRadioButtonChecked(object sender, RoutedEventArgs e)
        {
            ElementTheme selectedTheme = (ElementTheme)((RadioButton)sender).Tag;

            Grid content = MainPage.Current.Content as Grid;
            if (content is not null)
            {
                content.RequestedTheme = selectedTheme;
                SampleConfig.CurrentTheme = content.RequestedTheme;
            }
        }
    }
}
