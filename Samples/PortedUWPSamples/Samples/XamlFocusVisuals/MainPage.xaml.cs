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

using System;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// The shell page. Hosts a NavigationView whose menu items are the sample's scenarios,
    /// a Frame for the active scenario, and an InfoBar used for status/error messages.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public static MainPage Current;

        public MainPage()
        {
            this.InitializeComponent();

            // Allow downstream scenario pages to reach this instance (e.g. to call NotifyUser).
            Current = this;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Populate the navigation menu from the scenario list (SampleConfiguration.cs).
            int i = 1;
            foreach (Scenario s in scenarios)
            {
                NavView.MenuItems.Add(new NavigationViewItem
                {
                    Content = $"{i++}) {s.Title}",
                    Tag = s.ClassType
                });
            }

            if (NavView.MenuItems.Count > 0)
            {
                NavView.SelectedItem = NavView.MenuItems[0];
            }
        }

        private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            NotifyUser(string.Empty, NotifyType.StatusMessage);

            if (args.SelectedItem is NavigationViewItem item && item.Tag is Type scenarioType)
            {
                ScenarioFrame.Navigate(scenarioType);
            }
        }

        /// <summary>
        /// Display a message to the user. May be called from any thread.
        /// </summary>
        public void NotifyUser(string strMessage, NotifyType type)
        {
            // CoreDispatcher is replaced by DispatcherQueue in WinUI 3.
            if (DispatcherQueue.HasThreadAccess)
            {
                UpdateStatus(strMessage, type);
            }
            else
            {
                DispatcherQueue.TryEnqueue(() => UpdateStatus(strMessage, type));
            }
        }

        private void UpdateStatus(string strMessage, NotifyType type)
        {
            if (string.IsNullOrEmpty(strMessage))
            {
                StatusInfoBar.IsOpen = false;
                StatusInfoBar.Message = string.Empty;
                return;
            }

            StatusInfoBar.Severity = type switch
            {
                NotifyType.ErrorMessage => InfoBarSeverity.Error,
                _ => InfoBarSeverity.Success,
            };
            StatusInfoBar.Message = strMessage;
            StatusInfoBar.IsOpen = true;
        }
    }

    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    }
}

