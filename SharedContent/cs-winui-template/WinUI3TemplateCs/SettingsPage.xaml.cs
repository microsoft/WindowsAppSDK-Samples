﻿//*********************************************************
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
            if (Settings.CurrentTheme is null)
            {
                Settings.CurrentTheme = RequestedTheme.ToString();
            }

            themePanel.Children.Cast<RadioButton>().FirstOrDefault(c => c?.Tag?.ToString() == Settings.CurrentTheme).IsChecked = true;

            base.OnNavigatedTo(e);
        }

        private void OnThemeRadioButtonChecked(object sender, RoutedEventArgs e)
        {
            string selectedTheme = ((RadioButton)sender)?.Tag?.ToString();
            if (selectedTheme is not null)
            {
                Grid content = MainPage.Current.Content as Grid;
                if (content is not null)
                {
                    content.RequestedTheme = GetEnum<ElementTheme>(selectedTheme);
                    Settings.CurrentTheme = content.RequestedTheme.ToString();
                }
            }
        }

        private static TEnum GetEnum<TEnum>(string text) where TEnum : struct
        {
            if (!typeof(TEnum).GetTypeInfo().IsEnum)
            {
                throw new InvalidOperationException("Generic parameter 'TEnum' must be an enum.");
            }

            return (TEnum)Enum.Parse(typeof(TEnum), text);
        }
    }
}