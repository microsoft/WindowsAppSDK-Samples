// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;

namespace WinUI_CS_ShareTargetSampleApp
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            if (e.Parameter is List<string>)
            {
                var items = e.Parameter as List<string>;
                var text = $"File(s) shared with this app:\n{string.Join(Environment.NewLine, items)}";
                outputTextBlock.Text = text;
            }
        }
    }
}
