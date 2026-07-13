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

using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;

namespace SDKTemplate
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Gets the application's main window. WinUI 3 desktop has no <c>Window.Current</c>,
        /// so the sample tracks the main window here (migration: Window.Current -> App.MainWindow).
        /// </summary>
        public static Window MainWindow { get; private set; }

        /// <summary>
        /// Gets the DispatcherQueue for the UI thread. Replaces UWP's CoreDispatcher.
        /// </summary>
        public static DispatcherQueue MainDispatcherQueue { get; private set; }

        public App()
        {
            InitializeComponent();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            // NOTE: If this sample shows toasts, register the notification manager here
            // (AppNotificationManager.Default.Register();) and add the toast COM activator
            // to Package.appxmanifest. See AGENTS.md.

            var window = new MainWindow();
            MainWindow = window;
            MainDispatcherQueue = window.DispatcherQueue;

            // Navigate only after MainWindow is assigned: the shell's OnNavigatedTo reads App.MainWindow.
            window.NavigateToMainPage();
            window.Activate();
        }
    }
}
