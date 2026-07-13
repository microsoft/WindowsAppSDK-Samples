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
using Windows.System;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates launching a URI with the default handler, with a warning prompt,
    /// or with an Open With dialog.
    /// </summary>
    public sealed partial class Scenario2_LaunchUri : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        public Scenario2_LaunchUri()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Launch a URI.
        /// </summary>
        private async void LaunchUriDefault()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Launch a URI. Show a warning prompt.
        /// </summary>
        private async void LaunchUriWithWarning()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the warning prompt.
            var options = new LauncherOptions() { TreatAsUntrusted = true };

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri, options);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Launch a URI. Show an Open With dialog that lets the user choose the handler to use.
        /// </summary>
        private async void LaunchUriOpenWith()
        {
            // Create the URI to launch from a string.
            var uri = new Uri(UriToLaunch.Text);

            // Configure the Open With dialog.
            var options = new LauncherOptions();
            options.DisplayApplicationPicker = true;

            // Launch the URI.
            bool success = await Launcher.LaunchUriAsync(uri, options);
            if (success)
            {
                rootPage.NotifyUser("URI launched: " + uri.AbsoluteUri, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("URI launch failed.", NotifyType.ErrorMessage);
            }
        }
    }
}
