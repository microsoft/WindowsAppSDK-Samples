// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using WinRT;

namespace Windowing
{
    public partial class WindowBasics : Page
    {
        private AppWindow _mainAppWindow = MainWindow.MyAppWindow;
        public WindowBasics()
        {
            this.InitializeComponent();
        }

        private async void TitleBtn_Click(object sender, RoutedEventArgs e)
        {
            if (TitleTextBox.Text.Contains("\n"))
            {
                ContentDialog errorDialog = new ContentDialog()
                {
                    Title = "Single line text only!",
                    Content = "Only a single line of text is supported for a window titlebar. Please modify your text accordingly.",
                    CloseButtonText = "Ok"
                };
                ContentDialogResult result = await errorDialog.ShowAsync();
            }
            else
            {
                _mainAppWindow.Title = TitleTextBox.Text;
            }
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
