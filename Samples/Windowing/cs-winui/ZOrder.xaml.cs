﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI;
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


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Windowing
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ZOrder : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;
        List<Window> windowList = new List<Window>();

        public ZOrder()
        {
            this.InitializeComponent();
        }

        private void SwitchZOrder(object sender, RoutedEventArgs e)
        {
            // Bail out if we don't have an AppWindow object.
            if (_mainAppWindow != null)
            {
                switch (sender.As<Button>().Name)
                {
                    case "BottomZOrderBtn":
                        _mainAppWindow.MoveInZOrderAtBottom();
                        break;

                    case "TopZOrderBtn":
                        _mainAppWindow.MoveInZOrderAtTop();
                        break;

                    default:
                        
                        break;
                }
            }
        }

        private void myNewWindowButton_Click(object sender, RoutedEventArgs e)
        {
            Window nextWindow = new Window();
            nextWindow.Content = new TextBlock() { Text = "A wild window appears!" };
            nextWindow.Title = "Window #" + windowList.Count.ToString();
            nextWindow.Activate();
            windowList.Add(nextWindow);
            myListBox.Items.Add(nextWindow.Title);
            myOtherListBox.Items.Add(nextWindow.Title);
        }

        private void myFromIndexToTopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & myListBox.SelectedIndex != -1 & windowList.Any())
            {
                Window OtherWindow = windowList[myListBox.SelectedIndex];
                var otherhWnd = WinRT.Interop.WindowNative.GetWindowHandle(OtherWindow);
                Microsoft.UI.WindowId otherWindowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(otherhWnd);
                Microsoft.UI.Windowing.AppWindow otherAppWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(otherWindowId);
                otherAppWindow.MoveInZOrderBelow(_mainAppWindow.Id);
            }
        }

        private void myMoveBelowIndexBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & myOtherListBox.SelectedIndex != -1 & windowList.Any())
            {
                Window OtherWindow = windowList[myOtherListBox.SelectedIndex];
                var otherhWnd = WinRT.Interop.WindowNative.GetWindowHandle(OtherWindow);
                Microsoft.UI.WindowId otherWindowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(otherhWnd);
                _mainAppWindow.MoveInZOrderBelow(otherWindowId);

            }
        }
    }
}
