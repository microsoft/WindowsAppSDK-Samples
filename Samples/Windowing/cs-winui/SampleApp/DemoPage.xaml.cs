using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT;


namespace SampleApp
{
    public sealed partial class DemoPage : Page
    {
       
        AppWindow m_mainAppWindow;
        Window m_mainWindow;
        public DemoPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = e.Parameter.As<Window>().GetAppWindow();
            m_mainWindow = e.Parameter as MainWindow;

            base.OnNavigatedTo(e);
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


        private async void AppWindow_Closing(AppWindow sender, AppWindowClosingEventArgs args)
        {
            ContentDialog interceptCloseDialog = new ContentDialog()
            {
                Title = "Close intercepted!",
                Content = "Do you want to close the window?",
                CloseButtonText = "Yes",
                PrimaryButtonText = "No",
                DefaultButton = ContentDialogButton.Close
            };

            interceptCloseDialog.XamlRoot = m_mainWindow.Content.XamlRoot;
            ContentDialogResult result = await interceptCloseDialog.ShowAsync();

            if(result != ContentDialogResult.Primary)
            {
                // The user either dismissed with dialog (Esc/close), or selected No to go back to the app.
                // Cancel the system close.
                args.Cancel = true;
            }
        }

        private void InterceptCloseSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (sender.As<ToggleSwitch>().IsOn)
            {
                ShowMessageOnCloseSwitch.IsEnabled = true;
                m_mainAppWindow.Closing += AppWindow_Closing;
            }
            else
            {
                ShowMessageOnCloseSwitch.IsEnabled = false;
                m_mainAppWindow.Closing -= AppWindow_Closing;
            }
        }
    }
}
