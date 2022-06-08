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
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT;

namespace Windowing
{
    // Support class to create a databiding object we can use to connect our ListBox entries to our windows.
    public class WindowInfo
    {
        public string Title { get; set; }
        public WindowId Id { get; set; }
    }

    public sealed partial class ZOrder : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;

        // This is used give our windows unique titles.
        private int windowCount = 1;
        // We need an observable collection as we are databinding this to a ListBox.
        ObservableCollection<AppWindow> windowList = new ObservableCollection<AppWindow>();

        public ZOrder()
        {
            this.InitializeComponent();
            // Register for the closing event, we need to remove this window from our list of open windows when it goes away.
            _mainAppWindow.Closing += appWindow_Closing;
            
            // Set up a handler for when selection changes in our list box, so we can disable/enable the proper buttons.
            windowsListBox.SelectionChanged += MyListBox_SelectionChanged;

            // Add our main window to our list of windows and set up a databinding from the list to the ListBox.
            windowList.Add(_mainAppWindow);
            windowsListBox.ItemsSource = windowList;
        }

        private void MyListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // We can only change the z-order if we have selected a window, we enable/disable the buttons accordingly.
            if (windowsListBox.SelectedIndex == -1)
            {
                // We lost selection, disable all buttons.
                myFromIndexToTopBtn.IsEnabled = false;
                myMoveBelowIndexBtn.IsEnabled = false;
                TopZOrderBtn.IsEnabled = false;
                BottomZOrderBtn.IsEnabled = false;
            }
            else if (windowsListBox.SelectedIndex == 0)
            {
                // User has selected the "main" appwindow, disable "move below this window" and "move this below", enable all other
                myMoveBelowIndexBtn.IsEnabled = false;
                myFromIndexToTopBtn.IsEnabled = false;
                TopZOrderBtn.IsEnabled = true;
                BottomZOrderBtn.IsEnabled = true;
            }
            else // if (windowsListBox.SelectedIndex != -1)
            {
                // User has selected a "secondary window", enable all buttons.
                myMoveBelowIndexBtn.IsEnabled = true;
                myFromIndexToTopBtn.IsEnabled = true;
                TopZOrderBtn.IsEnabled = true;
                BottomZOrderBtn.IsEnabled = true;
            }
        }

        private void SwitchZOrder(object sender, RoutedEventArgs e)
        {
            // Bail out if we don't have an item selected.
            if (windowsListBox.SelectedIndex != -1)
            {
                switch (sender.As<Button>().Name)
                {
                    case "BottomZOrderBtn":
                        windowList[windowsListBox.SelectedIndex].MoveInZOrderAtBottom();
                        break;

                    case "TopZOrderBtn":
                        windowList[windowsListBox.SelectedIndex].MoveInZOrderAtTop();
                        break;

                    default:
                        break;
                }
            }
        }

        // Creates a new window 
        private void myNewWindowButton_Click(object sender, RoutedEventArgs e)
        {
            // Count up the number of windows we've spawned so far...
            windowCount++;
            
            // Create a new WinUI window
            Window secondaryWindow = new Window();
            secondaryWindow.Content = new TextBlock() { TextWrapping=TextWrapping.Wrap, HorizontalAlignment=HorizontalAlignment.Center, VerticalAlignment=VerticalAlignment.Center, Text = "This window is intentionally left blank, it is only used to illustrate z-order management using the controls available in the app's main window" };
            secondaryWindow.Title = "Window #" + windowCount.ToString();
            
            // Get the WindowId for the new XAML Window and store it in a list of known "secondary" windows for the app.
            Microsoft.UI.WindowId secondaryWindowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(WinRT.Interop.WindowNative.GetWindowHandle(secondaryWindow));

            // Get the AppWindow and register for the closing event so we can clean up our list of windows
            Microsoft.UI.Windowing.AppWindow secondaryAppWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(secondaryWindowId);

            // Add our window to the windowList
            windowList.Add(secondaryAppWindow);

            secondaryAppWindow.Closing += appWindow_Closing;
            // Set a more reasonable window size than the default one used by WinUI/XAML. ;)
            secondaryAppWindow.ResizeClient(new Windows.Graphics.SizeInt32(500, 500));
            
            // And activate our window
            secondaryWindow.Activate();
        }

        private void appWindow_Closing(AppWindow sender, AppWindowClosingEventArgs args)
        {

            // Remove this window from our list
            windowList.Remove(windowList.Where(z => z.Title == sender.Title).FirstOrDefault());
        }

        // Takes selected window and places it directly below the main window in the zorder stack
        private void myFromIndexToTopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & windowsListBox.SelectedIndex != -1 & windowList.Any())
            {
                windowList[windowsListBox.SelectedIndex].MoveInZOrderBelow(_mainAppWindow.Id);
            }
        }

        // Takes main window and places it directly below selected window in the zorder stack
        private void myMoveBelowIndexBtn_Click(object sender, RoutedEventArgs e)
        {
            if (_mainAppWindow != null & windowsListBox.SelectedIndex != -1 & windowList.Any())
            {
                _mainAppWindow.MoveInZOrderBelow(windowList[windowsListBox.SelectedIndex].Id);
            }
        }
    }
}
