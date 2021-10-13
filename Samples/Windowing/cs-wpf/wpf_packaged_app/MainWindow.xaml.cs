// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Windows.UI;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using Color = Windows.UI.Color;
using Microsoft.UI.Xaml.Controls;
using System.Xml;
using System.Windows.Data;

namespace wpf_packaged_app
{
    public partial class MainWindow : Window
    {
        public String m_windowTitle = "Wpf Desktop C# Sample App";
        AppWindow m_mainAppWindow;

        public MainWindow()
        {
            this.InitializeComponent();
            this.Title = m_windowTitle;

            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(this);

            Main.Content = new DemoPage();
        }

        private void Basics_Selected(object sender, RoutedEventArgs e)
        {
            Main.Content = new DemoPage();
        }

        private void Presenters_Selected(object sender, RoutedEventArgs e)
        {
            Main.Content = new PresenterPage();
        }

        private void Configs_Selected(object sender, RoutedEventArgs e)
        {
            Main.Content = new ConfigPage();
        }

        private void Titlebar_Selected(object sender, RoutedEventArgs e)
        {
            Main.Content = new TitleBarPage();
        }

        private void MyWindowIcon_MouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2)
            {
                this.Close();
            }
        }
    }
}
