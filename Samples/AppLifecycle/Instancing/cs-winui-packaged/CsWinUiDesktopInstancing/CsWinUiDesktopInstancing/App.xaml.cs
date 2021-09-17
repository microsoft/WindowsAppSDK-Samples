// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.UI.Xaml;
using System;
using System.Diagnostics;
using Windows.ApplicationModel.Activation;

namespace CsWinUiDesktopInstancing
{
    public partial class App : Application
    {
        private Window m_window;
        public Window AppWindow
        {
            get { return m_window; }
            private set { }
        }           

        public App()
        {
            InitializeComponent();
        }

        // NOTE: WinUI's App.OnLaunched is given a Microsoft.UI.Xaml.LaunchActivatedEventArgs,
        // whereas AppInstance.GetActivatedEventArgs will return a
        // Windows.ApplicationModel.Activation.LaunchActivatedEventArgs.
        // NOTE: All other *ActivatedEventArgs are not ambiguous, as they're only
        // defined in Windows.ApplicationModel.Activation.
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            // NOTE AppInstance is ambiguous:
            // Microsoft.Windows.AppLifecycle.AppInstance
            // Windows.ApplicationModel.AppInstance
            var currentInstance =
                Microsoft.Windows.AppLifecycle.AppInstance.GetCurrent();
            if (currentInstance != null)
            {
                var activationArgs = currentInstance.GetActivatedEventArgs();
                if (activationArgs != null)
                {
                    if (activationArgs.Kind ==
                        Microsoft.Windows.AppLifecycle.ExtendedActivationKind.Launch)
                    {
                        if (activationArgs.Data is ILaunchActivatedEventArgs launchArgs)
                        {
                            Debug.WriteLine($"LaunchArgs: {launchArgs.Arguments}");
                        }
                        Debug.WriteLine($"CommandLine: {Environment.CommandLine}");
                        string[] cmdArgs = Environment.GetCommandLineArgs();
                        foreach (string cmdArg in cmdArgs)
                        {
                            Debug.WriteLine($"GetCommandLineArgs: {cmdArg}");
                        }
                    }

                }
            }

            // Go ahead and do standard window initialization regardless.
            m_window = new MainWindow();
            m_window.Activate();
        }

    }
}
