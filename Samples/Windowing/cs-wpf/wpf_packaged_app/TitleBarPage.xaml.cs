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
        private bool _isTallTitleBar = false;

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

                // Enable title bar height toggle buttons
                this.StandardHeightBtn.IsEnabled = true;
                this.TallHeightBtn.IsEnabled = true;

                // Set Button colors to match the custom titlebar
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                // Set the drag region for the custom title bar
                SetDragRegionForCustomTitleBar(_mainAppWindow);
            }
            else
            {
                // Bring back the default titlebar
                _mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;

                // Disable title bar height toggle buttons
                this.StandardHeightBtn.IsEnabled = false;
                this.TallHeightBtn.IsEnabled = false;

                // reset the title bar to default state
                _mainAppWindow.TitleBar.ResetToDefault();
            }
        }

        private void SetDragRegionForCustomTitleBar(AppWindow appWindow)
        {
            //Infer titlebar height
            int titleBarHeight = appWindow.TitleBar.Height;
            _mainWindow.MyTitleBar.Height = titleBarHeight;

            // Get caption button occlusion information
            // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
            int CaptionButtonOcclusionWidth = appWindow.TitleBar.RightInset;

            // Define your drag Regions
            int windowIconWidthAndPadding = (int)_mainWindow.MyWindowIcon.ActualWidth + (int)_mainWindow.MyWindowIcon.Margin.Right;
            int dragRegionWidth = appWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

            Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
            Windows.Graphics.RectInt32 dragRect;

            dragRect.X = windowIconWidthAndPadding;
            dragRect.Y = 0;
            dragRect.Height = titleBarHeight;
            dragRect.Width = dragRegionWidth;

            appWindow.TitleBar.SetDragRectangles(dragRects.Append(dragRect).ToArray());
        }

        private void StandardHeightBtn_Click(object sender, RoutedEventArgs e)
        {
            _isTallTitleBar = false;
            toggleTitlebarHeightOption();
        }
        private void TallHeightBtn_Click(object sender, RoutedEventArgs e)
        {
            _isTallTitleBar = true;
            toggleTitlebarHeightOption();
        }
        private void toggleTitlebarHeightOption()
        {
            // A taller title bar is only supported when drawing a fully custom title bar
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                if (_isTallTitleBar)
                {
                    // Choose a tall title bar to provide more room for interactive elements like searchboxes, person pictures e.t.c
                    _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
                }
                else
                {
                    _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Standard;
                }
                // Reset the drag region for the custom title bar
                SetDragRegionForCustomTitleBar(_mainAppWindow);

            }
            else
            {
                this.StandardHeightBtn.IsEnabled = false;
                this.TallHeightBtn.IsEnabled = false;
            }
        }
    }
}
