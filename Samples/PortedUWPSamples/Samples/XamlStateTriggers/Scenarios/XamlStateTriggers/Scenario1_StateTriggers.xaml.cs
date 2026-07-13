//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using SDKTemplate.ViewModel;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates built-in and custom XAML state triggers.
    /// </summary>
    public sealed partial class Scenario1_StateTriggers : Page
    {
        private MainViewModel _viewModel;

        public Scenario1_StateTriggers()
        {
            this.InitializeComponent();
            this.Loaded += Scenario1_StateTriggers_Loaded;
        }

        void Scenario1_StateTriggers_Loaded(object sender, RoutedEventArgs e)
        {
            _viewModel = new MainViewModel();
            list.ItemsSource = _viewModel.Items;
        }
    }
}
