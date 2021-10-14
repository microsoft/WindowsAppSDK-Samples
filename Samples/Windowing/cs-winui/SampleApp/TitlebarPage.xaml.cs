// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
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
using Microsoft.UI;
using Windows.UI;


namespace SampleApp
{
    public sealed partial class TitlebarPage : Page
    {
        private AppWindow m_mainAppWindow;
        private bool m_isBrandedTitleBar;
        private MainWindow m_mainWindow;

        public TitlebarPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = e.Parameter.As<Window>().GetAppWindow();

            m_mainWindow = e.Parameter as MainWindow;
            m_mainAppWindow.Changed += MainAppWindow_Changed;
            base.OnNavigatedTo(e);
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
                m_mainAppWindow.Title = "Default titlebar";
            }
            m_mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
        } 
        
        private void ResetTitlebarBtn_Click(object sender, RoutedEventArgs e)
        {
            m_mainAppWindow.TitleBar.ResetToDefault();
            m_mainWindow.Title = m_mainWindow.m_windowTitle;
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

                // Set the drag region for the custom TitleBar
                SetDragRegionForCustomTitleBar(m_mainAppWindow);
            }
            else
            {
                // Bring back the default titlebar
                m_mainWindow.MyTitleBar.Visibility = Visibility.Collapsed;
                m_mainAppWindow.TitleBar.ResetToDefault();
            }
        }

        private void SetDragRegionForCustomTitleBar(AppWindow appWindow)
        {
            //Infer titlebar height
            int titleBarHeight = appWindow.TitleBar.Height;
            m_mainWindow.MyTitleBar.Height = titleBarHeight;

            // Get caption button occlusion information
            // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
            int CaptionButtonOcclusionWidth = appWindow.TitleBar.RightInset;

            // Define your drag Regions
            int windowIconWidthAndPadding = (int)m_mainWindow.MyWindowIcon.ActualWidth + (int)m_mainWindow.MyWindowIcon.Margin.Right;
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