// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.
using System;
using System.Windows;
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
        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowHandleFromWindowId(WindowId windowId, out IntPtr result);

        [DllImport("Microsoft.UI.Windowing.Core.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindowHandle(IntPtr hwnd, out WindowId result);

        public String m_windowTitle = "Wpf Desktop C# Sample App";
        AppWindow m_mainAppWindow;

        public static Microsoft.UI.Windowing.AppWindow GetAppWindowFromWPFWindow(Window wpfWindow)
        {
            // Get the HWND of the top level WPF window.
            var helper = new WindowInteropHelper(wpfWindow);
            IntPtr hwnd = (HwndSource.FromHwnd(helper.EnsureHandle())
                as IWin32Window).Handle;

            // Get the WindowId from the HWND.
            Microsoft.UI.WindowId windowId;
            GetWindowIdFromWindowHandle(hwnd, out windowId);

            // Get an AppWindow from the WindowId.
            Microsoft.UI.Windowing.AppWindow appWindow =
            Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);

            return appWindow;
        }
        public MainWindow()
        {
            this.InitializeComponent();
            this.Title = m_windowTitle;

            // Get appwindow from HWND
            m_mainAppWindow = GetAppWindowFromWPFWindow(this);
            Main.Content = new DemoPage();
        }


        private void ListBoxItem_Selected(object sender, RoutedEventArgs e)
        {
            Main.Content = new DemoPage();
        }

        private void ListBoxItem_Selected_1(object sender, RoutedEventArgs e)
        {
            Main.Content = new PresenterPage();
        }

        private void ListBoxItem_Selected_2(object sender, RoutedEventArgs e)
        {
            Main.Content = new ConfigPage();
        }

        private void ListBoxItem_Selected_3(object sender, RoutedEventArgs e)
        {
            Main.Content = new TitleBarPage();
        }
    }
}
