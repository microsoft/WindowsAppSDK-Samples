// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using WinRT;

namespace Windowing
{
    public partial class WindowBasics : Page
    {
        AppWindow m_mainAppWindow;
        public WindowBasics()
        {
            this.InitializeComponent();
            Window window = MainWindow.Current;
            m_mainAppWindow = AppWindowExtensions.GetAppWindow(window);
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
                m_mainAppWindow.Title = TitleTextBox.Text;
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

            if (windowHeight > 0 && windowWidth > 0)
            {
                m_mainAppWindow.Resize(new Windows.Graphics.SizeInt32(windowWidth, windowHeight));
            }
        }
    }
}
