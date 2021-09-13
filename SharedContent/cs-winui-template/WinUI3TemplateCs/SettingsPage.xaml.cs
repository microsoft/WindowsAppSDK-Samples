//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Linq;
using System.Reflection;

namespace WinUI3TemplateCs
{
    public partial class SettingsPage : Page
    {
        public SettingsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            themePanel.Children.Cast<RadioButton>().First(c => (ElementTheme)c.Tag == Settings.CurrentTheme).IsChecked = true;
            base.OnNavigatedTo(e);
        }

        private void OnThemeRadioButtonChecked(object sender, RoutedEventArgs e)
        {
            ElementTheme selectedTheme = (ElementTheme)((RadioButton)sender).Tag;
      
                Grid content = MainPage.Current.Content as Grid;
                if (content is not null)
                {
                    content.RequestedTheme = selectedTheme;
                    Settings.CurrentTheme = content.RequestedTheme;
                }
            
        }
    }
}
