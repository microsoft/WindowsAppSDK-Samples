// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using Microsoft.UI;
using Microsoft.UI.Windowing;

namespace wpf_packaged_app
{
    public partial class TitleBarPage : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;
        private MainWindow _mainWindow;
        private bool _isBrandedTitleBar = false;

        public TitleBarPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            _mainWindow = Window.GetWindow(this) as MainWindow;
        }

        private void TitlebarBrandingBtn_Click(object sender, RoutedEventArgs e)
        {
            _mainAppWindow.TitleBar.ResetToDefault();

            _isBrandedTitleBar = !_isBrandedTitleBar;
            // Check to see if customization is supported. Currently only supported on Windows 11.
            if (AppWindowTitleBar.IsCustomizationSupported() && _isBrandedTitleBar)
            {
                _mainAppWindow.Title = "Default titlebar with custom color customization";
                _mainAppWindow.TitleBar.ForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.BackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.InactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.InactiveForegroundColor = Colors.White;

                // Buttons
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.DarkOrange;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;
            }
            else
            {
                _mainAppWindow.Title = MainWindow.WindowTitle;
            }
            _mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void ResetTitlebarBtn_Click(object sender, RoutedEventArgs e)
        {
            _mainAppWindow.TitleBar.ResetToDefault();
            _mainAppWindow.Title = MainWindow.WindowTitle;
            _mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void TitlebarCustomBtn_Click(object sender, RoutedEventArgs e)
        {
            _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = !_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar;

            // Check to see if customization is supported. Currently only supported on Windows 11.
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Show the custom titlebar
                _mainWindow.MyTitleBar.Visibility = Visibility.Visible;

                // Set Button colors to match the custom titlebar
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                //Infer titlebar height
                int titleBarHeight = _mainAppWindow.TitleBar.Height;
                _mainWindow.MyTitleBar.Height = titleBarHeight;

                // Get caption button occlusion information
                // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
                int CaptionButtonOcclusionWidth = _mainAppWindow.TitleBar.RightInset;

                // Define your drag Regions
                int windowIconWidthAndPadding = (int)_mainWindow.MyWindowIcon.ActualWidth + (int)_mainWindow.MyWindowIcon.Margin.Right;
                int dragRegionWidth = _mainAppWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

                Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
                Windows.Graphics.RectInt32 dragRect;

                dragRect.X = windowIconWidthAndPadding;
                dragRect.Y = 0;
                dragRect.Height = titleBarHeight;
                dragRect.Width = dragRegionWidth;

                var dragRectsArray = dragRects.Append(dragRect).ToArray();
                _mainAppWindow.TitleBar.SetDragRectangles(dragRectsArray);
            }
            else
            {
                // Bring back the default titlebar
                _mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
                _mainAppWindow.TitleBar.ResetToDefault();
            }
        }
    }
}
