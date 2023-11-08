// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.UI.Xaml;

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
        // where the UWPLaunchActivatedEventArgs property will be one of the 
        // Windows.ApplicationModel.Activation.*ActivatedEventArgs types.
        // Conversely, AppInstance.GetActivatedEventArgs will return a
        // Microsoft.Windows.AppLifecycle.AppActivationArguments, where the Data property
        // will be one of the Windows.ApplicationModel.Activation.*ActivatedEventArgs types.
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            // NOTE: OnLaunched will always report that the ActivationKind == Launch,
            // even when it isn't.
            Windows.ApplicationModel.Activation.ActivationKind kind
                = args.UWPLaunchActivatedEventArgs.Kind;
            Program.ReportInfo($"OnLaunched: Kind={kind}");

            // NOTE: AppInstance is ambiguous between
            // Microsoft.Windows.AppLifecycle.AppInstance and
            // Windows.ApplicationModel.AppInstance
            var currentInstance =
                Microsoft.Windows.AppLifecycle.AppInstance.GetCurrent();
            if (currentInstance != null)
            {
                // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
                // even in WinUI's OnLaunched.
                Microsoft.Windows.AppLifecycle.AppActivationArguments activationArgs 
                    = currentInstance.GetActivatedEventArgs();
                if (activationArgs != null)
                {
                    Microsoft.Windows.AppLifecycle.ExtendedActivationKind extendedKind
                        = activationArgs.Kind;
                    Program.ReportInfo($"activationArgs.Kind={extendedKind}");
                }
            }

            // Go ahead and do standard window initialization regardless.
            m_window = new MainWindow();
            m_window.Activate();
        }
    }
}
