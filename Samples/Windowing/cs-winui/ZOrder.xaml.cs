// Copyright (c) Microsoft Corporation.
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
        private int m_windowCount = 0;
        List<WindowId> windowList = new List<WindowId>();

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
            // Count up the number of windows created so far, this is just used for Title generation so we don't collide
            m_windowCount++;
            Window nextWindow = new Window();
            nextWindow.Content = new TextBlock() { Text = "A wild window appears!" };
            nextWindow.Title = "Window #" + m_windowCount.ToString();
            nextWindow.Activate();
            
            // Get the WindowId for the new XAML Window and store it in a list of known "secondary" windows for the app.
            var nextWindowHWnd = WinRT.Interop.WindowNative.GetWindowHandle(nextWindow);
            Microsoft.UI.WindowId nextWindowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(nextWindowHWnd);

            windowList.Add(nextWindowId);
            myListBox.Items.Add(nextWindow.Title);
            myOtherListBox.Items.Add(nextWindow.Title);

            Microsoft.UI.Windowing.AppWindow nextAppWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(nextWindowId);
            nextAppWindow.Closing += NextAppWindow_Closing;
        }

        private void NextAppWindow_Closing(AppWindow sender, AppWindowClosingEventArgs args)
        {
            // Find the window that is closing in our list of WindowIds and remove it from
            // that list as well as the listboxes to prevent the user from selecting a window that is no longer available.
            int index = windowList.FindIndex(x => x == sender.Id);
            if(index != -1)
            {
                windowList.RemoveAt(index);
                myListBox.Items.RemoveAt(index);
                myOtherListBox.Items.RemoveAt(index);
                if(index == windowList.Count)
                {
                    m_windowCount--;
                }
            }
        }

        private void myFromIndexToTopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & myListBox.SelectedIndex != -1 & windowList.Any())
            {
                WindowId otherWindowId = windowList[myListBox.SelectedIndex];
                AppWindow otherAppWindow = AppWindow.GetFromWindowId(otherWindowId);
                otherAppWindow.MoveInZOrderBelow(_mainAppWindow.Id);
            }
        }

        private void myMoveBelowIndexBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & myOtherListBox.SelectedIndex != -1 & windowList.Any())
            {
                WindowId otherWindowId = windowList[myOtherListBox.SelectedIndex];
                _mainAppWindow.MoveInZOrderBelow(otherWindowId);

            }
        }
    }
}
