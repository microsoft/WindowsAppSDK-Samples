// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Windows;
using System.Windows.Controls;
using Microsoft.UI.Windowing;
using WinRT;

namespace wpf_packaged_app
{
    public partial class DemoPage : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;

        public DemoPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
        }

        private void TitleBtn_Click(object sender, RoutedEventArgs e)
        {
            _mainAppWindow.Title = TitleTextBox.Text;          
        }

        private void SizeBtn_Click(object sender, RoutedEventArgs e)
        {
            int windowWidth = 0;
            int windowHeight = 0;

            try
            {
                windowWidth = int.Parse(WidthTextBox.Text);
                windowHeight = int.Parse(HeightTextBox.Text);
            }
            catch (FormatException)
            {
                // Silently ignore invalid input...
            }

            if (_mainAppWindow != null)
            {
                switch (sender.As<Button>().Name)
                {
                    case "SizeBtn":
                        _mainAppWindow.Resize(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
                        break;

                    case "ClientSizeBtn":
                        _mainAppWindow.ResizeClient(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
                        break;
                }
            }
        }
    }
}
