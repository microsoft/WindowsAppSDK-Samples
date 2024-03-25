// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.DataTransfer;

namespace WinUI_CS_ShareTargetSampleApp
{
    public partial class App : Application
    {
        public App()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when the application is launched.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override async void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            List<string> sharedFilePaths = null;

            // Note: In Package.appxmanifest, we declare that this app can be launched as a Share Target.

            // Check if this app was launched as a Share Target:
            var activationArguments = Windows.ApplicationModel.AppInstance.GetActivatedEventArgs();
            if(activationArguments.Kind == ActivationKind.ShareTarget)
            {
                ShareTargetActivatedEventArgs shareArgs = activationArguments as ShareTargetActivatedEventArgs;

                // Get the shared data from the ShareOperation:
                shareArgs.ShareOperation.ReportStarted();
                if (shareArgs.ShareOperation.Data.Contains(StandardDataFormats.StorageItems))
                {
                    var storageItems = await shareArgs.ShareOperation.Data.GetStorageItemsAsync();
                    sharedFilePaths = storageItems.Select(item => item.Path).ToList();
                }

                // Once we have received the shared data from the ShareOperation, call ReportCompleted()
                shareArgs.ShareOperation.ReportCompleted();
            }

            m_window = new MainWindow();
            Frame rootFrame = m_window.Content as Frame;
            rootFrame.Navigate(typeof(MainPage), sharedFilePaths);
            m_window.Activate();
        }

        private Window m_window;
    }
}
