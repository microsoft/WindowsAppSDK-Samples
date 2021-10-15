// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using WinRT;

namespace wpf_packaged_app
{
    public partial class TitleBarPage : Page
    {
        AppWindow m_mainAppWindow;
        MainWindow m_mainWindow;
        bool m_isBrandedTitleBar = false;

        public TitleBarPage()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            m_mainWindow = Window.GetWindow(this) as MainWindow;
            m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWPFWindow(m_mainWindow);

        }

        private void TitlebarBrandingBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();

            m_isBrandedTitleBar = !m_isBrandedTitleBar;
            if (m_isBrandedTitleBar)
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
                m_mainAppWindow.Title = m_mainWindow.m_windowTitle;
            }
            m_mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void ResetTitlebarBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();
            m_mainAppWindow.Title = m_mainWindow.m_windowTitle;
            m_mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
        }

        private void TitlebarCustomBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = !m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar;

            if (m_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Show the custom titlebar
                m_mainWindow.MyTitleBar.Visibility = Visibility.Visible;

                // Set Button colors to match the custom titlebar
                m_mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Blue;
                m_mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                m_mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                m_mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                //Infer titlebar height
                int titleBarHeight = m_mainAppWindow.TitleBar.Height;
                m_mainWindow.MyTitleBar.Height = titleBarHeight;

                // Get caption button occlusion information
                // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
                int CaptionButtonOcclusionWidth = m_mainAppWindow.TitleBar.RightInset;

                // Define your drag Regions
                int windowIconWidthAndPadding = (int)m_mainWindow.MyWindowIcon.ActualWidth + (int)m_mainWindow.MyWindowIcon.Margin.Right;
                int dragRegionWidth = m_mainAppWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

                Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
                Windows.Graphics.RectInt32 dragRect;

                dragRect.X = windowIconWidthAndPadding;
                dragRect.Y = 0;
                dragRect.Height = titleBarHeight;
                dragRect.Width = dragRegionWidth;

                var dragRectsArray = dragRects.Append(dragRect).ToArray();
                m_mainAppWindow.TitleBar.SetDragRectangles(dragRectsArray);

            }
            else
            {
                // Bring back the default titlebar
                m_mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
                m_mainAppWindow.TitleBar.ResetToDefault();
            }
        }
    }
}
