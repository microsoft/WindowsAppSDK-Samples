// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Windowing;
using WinRT;
using Microsoft.UI.Xaml.Navigation;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;

namespace Windowing
{
    public partial class TitleBar : Page
    {
        private AppWindow m_mainAppWindow;
        private bool m_isBrandedTitleBar;
        private MainWindow m_mainWindow;
        private MainPage m_mainPage;
        public TitleBar()
        {
            this.InitializeComponent();
            m_mainWindow = MainWindow.Current;
            m_mainPage= MainPage.Current;
            m_mainAppWindow = AppWindowExtensions.GetAppWindow(m_mainWindow);
            m_mainAppWindow.Changed += MainAppWindow_Changed;
        }
 

        private void MainAppWindow_Changed(AppWindow sender, AppWindowChangedEventArgs args)
        {
            if (args.DidSizeChange && sender.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Need to update our drag region if the size of the window changes
                SetDragRegionForCustomTitleBar(sender);
            }
        }

        private void TitlebarBrandingBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();

            m_isBrandedTitleBar = !m_isBrandedTitleBar;
            if (AppWindowTitleBar.IsCustomizationSupported() && m_isBrandedTitleBar)
            {
                m_mainAppWindow.Title = "Default titlebar with custom color customization";
                m_mainAppWindow.TitleBar.ForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.BackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.InactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.InactiveForegroundColor = Colors.White;

                //Buttons
                m_mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.DarkOrange;
                m_mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;
            }
            else
            {
                m_mainAppWindow.Title = "Default titlebar";
            }
            m_mainPage.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void ResetTitlebarBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();
            m_mainWindow.Title = m_mainWindow.m_windowTitle;
            m_mainPage.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void TitlebarCustomBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = !m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar;

            if (AppWindowTitleBar.IsCustomizationSupported() && m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Show the custom titlebar
                m_mainPage.MyTitleBar.Visibility = Visibility.Visible;

                // Set Button colors to match the custom titlebar
                m_mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                // Set the drag region for the custom TitleBar
                SetDragRegionForCustomTitleBar(m_mainAppWindow);
            }
            else
            {
                // Bring back the default titlebar
                m_mainPage.MyTitleBar.Visibility = Visibility.Collapsed;
                m_mainAppWindow.TitleBar.ResetToDefault();
            }
        }

        private void SetDragRegionForCustomTitleBar(AppWindow appWindow)
        {
            //Infer titlebar height
            int titleBarHeight = appWindow.TitleBar.Height;
            m_mainPage.MyTitleBar.Height = titleBarHeight;

            // Get caption button occlusion information
            // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
            int CaptionButtonOcclusionWidth = appWindow.TitleBar.RightInset;

            // Define your drag Regions
            int windowIconWidthAndPadding = (int)m_mainPage.MyWindowIcon.ActualWidth + (int)m_mainPage.MyWindowIcon.Margin.Right;
            int dragRegionWidth = appWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

            Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
            Windows.Graphics.RectInt32 dragRect;

            dragRect.X = windowIconWidthAndPadding;
            dragRect.Y = 0;
            dragRect.Height = titleBarHeight;
            dragRect.Width = dragRegionWidth;

            appWindow.TitleBar.SetDragRectangles(dragRects.Append(dragRect).ToArray());
        }
    }
}
