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
using System.Runtime.InteropServices;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Activation;

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

        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            var window = new MainWindow();
            MainWindow = window;
            MainDispatcherQueue = window.DispatcherQueue;

            // Navigate only after MainWindow is assigned: the shell's OnNavigatedTo reads App.MainWindow.
            window.NavigateToMainPage();
            window.Activate();

            // This instance may have been cold-started by a file or protocol activation (UWP's
            // OnFileActivated / OnActivated). WinUI 3 exposes that through AppInstance instead of
            // dedicated Application overrides, so inspect the activation arguments and route.
            AppActivationArguments activationArgs = AppInstance.GetCurrent().GetActivatedEventArgs();
            RouteActivation(activationArgs);
        }

        /// <summary>
        /// Called by <see cref="Program"/> when another instance redirects its activation to this,
        /// already-running instance. Runs on a background thread, so marshal to the UI thread.
        /// </summary>
        public static void HandleRedirectedActivation(AppActivationArguments args)
        {
            MainDispatcherQueue?.TryEnqueue(() =>
            {
                RouteActivation(args);
                BringToForeground();
            });
        }

        /// <summary>
        /// Inspects the activation kind and, for file/protocol activations, navigates the shell to
        /// the matching "Receiving" scenario, passing the activation payload as the parameter.
        /// </summary>
        private static void RouteActivation(AppActivationArguments args)
        {
            if (MainPage.Current == null)
            {
                return;
            }

            switch (args.Kind)
            {
                case ExtendedActivationKind.File:
                    if (args.Data is IFileActivatedEventArgs fileArgs)
                    {
                        MainPage.Current.NavigateToScenario(typeof(Scenario3_ReceiveFile), fileArgs);
                    }
                    break;

                case ExtendedActivationKind.Protocol:
                    if (args.Data is IProtocolActivatedEventArgs protocolArgs)
                    {
                        MainPage.Current.NavigateToScenario(typeof(Scenario4_ReceiveUri), protocolArgs);
                    }
                    break;

                // ExtendedActivationKind.Launch and all other kinds are a normal start; the shell
                // already selected the first scenario, so there is nothing extra to do.
            }
        }

        private static void BringToForeground()
        {
            if (MainWindow == null)
            {
                return;
            }

            IntPtr hwnd = WinRT.Interop.WindowNative.GetWindowHandle(MainWindow);
            if (IsIconic(hwnd))
            {
                ShowWindow(hwnd, SW_RESTORE);
            }
            SetForegroundWindow(hwnd);
        }

        private const int SW_RESTORE = 9;

        [DllImport("user32.dll")]
        private static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [DllImport("user32.dll")]
        private static extern bool IsIconic(IntPtr hWnd);
    }
}
