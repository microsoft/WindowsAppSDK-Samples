// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System.Windows;
using Microsoft.UI.Windowing;

namespace wpf_packaged_app
{
    public partial class MainWindow : Window
    {
        public static AppWindow AppWindow;
        public static string WindowTitle = "Windowing WPF C# Sample";

        public MainWindow()
        {
            this.InitializeComponent();
            Title = WindowTitle;

            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            AppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(this);   

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
