// Copyright (c) Microsoft Corporation. All rights reserved.
using Microsoft.UI.Xaml;

namespace Notes
{
    public partial class App : Application
    {
        public App()
        {
            //Initialize.AssemblyInitialize();
            this.InitializeComponent();
        }

        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            m_window = new MainWindow();
            m_window.Activate();
        }

        private Window? m_window;
        public Window? Window => m_window;
    }
}
