﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System.Windows;

namespace wpf_packaged_app
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
            this.InitializeComponent();
        }

        private void AppStartup(object sender, StartupEventArgs e)
        {
            m_mainWindow = new MainWindow();
            m_mainWindow.Activate();
        }
        private Window m_mainWindow;
    }
}