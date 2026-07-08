// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Windows.UI.ViewManagement;

namespace SDKTemplate
{
    /// <summary>
    /// Sample page to call TryShow/TryHide methods.
    /// </summary>
    public sealed partial class Scenario3_ShowHideMethods : Page
    {
        private InputPane _inputPane;

        public Scenario3_ShowHideMethods()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            var window = App.MainWindow;
            if (window is null)
            {
                return;
            }

            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(window);
            _inputPane = InputPaneInterop.GetForWindow(hwnd);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            FadeOutResults.Stop();
            _inputPane = null;
            base.OnNavigatedFrom(e);
        }

        private void WordListBox_OnKeyUp(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                string text = WordListBox.Text;
                if (text.Length > 0)
                {
                    _inputPane?.TryHide();
                    ResultsTextBlock.Text = text;
                    FadeOutResults.Begin();
                }
            }
        }

        private void OnFadeOutCompleted(object sender, object e)
        {
            ResultsTextBlock.Text = "";
            _inputPane?.TryShow();
        }
    }
}
