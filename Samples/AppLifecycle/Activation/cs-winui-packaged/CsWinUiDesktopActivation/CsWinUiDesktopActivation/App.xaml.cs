// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.UI.Xaml;

namespace CsWinUiDesktopActivation
{
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();
        }

        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            m_window = new MainWindow();
            m_window.Activate();
        }

        private Window m_window;
    }
}
