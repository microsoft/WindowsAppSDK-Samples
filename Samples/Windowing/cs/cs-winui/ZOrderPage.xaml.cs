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
        private AppWindow _mainAppWindow = MainWindow.MyAppWindow;
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

        // Creates a new window 
        private void myNewWindowButton_Click(object sender, RoutedEventArgs e)
        {
         
            Window secondaryWindow = new Window();
            secondaryWindow.Content = new TextBlock() { Text = "This window is intentionally left blank, it is only used to illustrate z-order management using the controls available in the app's main window" };
            secondaryWindow.Title = "Window #" + (windowList.Count + 1).ToString();
            secondaryWindow.Activate();
            
            // Get the WindowId for the new XAML Window and store it in a list of known "secondary" windows for the app.
            var secondaryWindowHWnd = WinRT.Interop.WindowNative.GetWindowHandle(secondaryWindow);
            Microsoft.UI.WindowId secondaryWindowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(secondaryWindowHWnd);

            // Adds a window to the windowList struct 
            windowList.Add(secondaryWindowId);

            // Adds the window title to the myListBox XAML Stackbox 
            secondaryWindowsListBox.Items.Add(secondaryWindow.Title);

            Microsoft.UI.Windowing.AppWindow nextAppWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(secondaryWindowId);
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
                secondaryWindowsListBox.Items.RemoveAt(index);

                for (int i = 0; i < windowList.Count; i++)
                {
                    var new_title = i + 1;
                    WindowId windowId = windowList[i];
                    AppWindow iteratedAppWindow = AppWindow.GetFromWindowId(windowId);
                    iteratedAppWindow.Title = "Window #" + new_title.ToString();
                    secondaryWindowsListBox.Items[i] = "Window #" + new_title.ToString();
                }
            }
        }

        // Takes selected window and places it directly below the main window in the zorder stack
        private void myFromIndexToTopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & secondaryWindowsListBox.SelectedIndex != -1 & windowList.Any())
            {
                WindowId otherWindowId = windowList[secondaryWindowsListBox.SelectedIndex];
                AppWindow otherAppWindow = AppWindow.GetFromWindowId(otherWindowId);
                otherAppWindow.MoveInZOrderBelow(_mainAppWindow.Id);
            }
        }

        // Takes main window and places it directly below selected window in the zorder stack
        private void myMoveBelowIndexBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & secondaryWindowsListBox.SelectedIndex != -1 & windowList.Any())
            {
                WindowId otherWindowId = windowList[secondaryWindowsListBox.SelectedIndex];
                _mainAppWindow.MoveInZOrderBelow(otherWindowId);

            }
        }
    }
}
