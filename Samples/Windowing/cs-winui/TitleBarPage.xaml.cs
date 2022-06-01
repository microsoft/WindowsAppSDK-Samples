// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Windowing;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System;
using Microsoft.UI.Xaml.Navigation;

namespace Windowing
{
    public partial class TitleBar : Page
    {
        private AppWindow _mainAppWindow = MainWindow.AppWindow;
        private MainPage _mainPage = MainPage.Current;
        public TitleBar()
        {
            this.InitializeComponent();
            _mainAppWindow.Changed += MainAppWindow_Changed;
        }

        private void MainAppWindow_Changed(AppWindow sender, AppWindowChangedEventArgs args)
        {
            if (args.DidSizeChange && sender.TitleBar.ExtendsContentIntoTitleBar)
            {
                // Need to update our drag region if the size of the window changes
                SetDragRegionForCustomTitleBar(sender);
            }
        }

        private void ResetTitlebar()
        {
            // Reset the titlebar to a standard system titlebar
            _mainAppWindow.TitleBar.ResetToDefault();

            // Set the title back to what we had set when we launched the app
            _mainAppWindow.Title = Settings.FeatureName;

            // Hide the custom titlebar XAML element
            _mainPage.MyTitleBar.Visibility = Visibility.Collapsed;

            // Turn all the switches back to their off state and disable the ones not compatible with the default titlebar
            tallTitleBarSwitch.IsOn = false;
            tallTitleBarSwitch.IsEnabled = false;
            titleCustomSwitch.IsOn = false;
            titleCustomSwitch.IsEnabled = true;
            titleBarBrandingSwitch.IsOn = false;
            titleBarBrandingSwitch.IsEnabled = true;
        }

        private void ResetTitlebarBtn_Click(object sender, RoutedEventArgs e)
        {
            ResetTitlebar();
        }

        private void SetDragRegionForCustomTitleBar(AppWindow appWindow)
        {
            //Infer titlebar height
            _mainPage.MyTitleBar.Height = appWindow.TitleBar.Height;

            // Get caption button occlusion information
            // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
            int CaptionButtonOcclusionWidth = appWindow.TitleBar.RightInset;

            // Define your drag Regions
            int windowIconWidthAndPadding = (int)_mainPage.MyWindowIcon.ActualWidth + (int)_mainPage.MyWindowIcon.Margin.Right;
            int dragRegionWidth = appWindow.Size.Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding);

            Windows.Graphics.RectInt32[] dragRects = new Windows.Graphics.RectInt32[] { };
            Windows.Graphics.RectInt32 dragRect;

            dragRect.X = windowIconWidthAndPadding;
            dragRect.Y = 0;
            dragRect.Height = appWindow.TitleBar.Height;
            dragRect.Width = dragRegionWidth;

            appWindow.TitleBar.SetDragRectangles(dragRects.Append(dragRect).ToArray());
        }

        private void tallTitleBarSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (AppWindowTitleBar.IsCustomizationSupported() && _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
            {
                ToggleSwitch toggleSwitch = sender as ToggleSwitch;
                if (toggleSwitch != null)
                {
                    if (toggleSwitch.IsOn == true)
                    {
                        // Choose a tall title bar to provide more room for interactive elements like searchboxes, person pictures e.t.c
                        _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
                    }
                    else
                    {
                        _mainAppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Standard;
                    }
                }
                // Reset the drag region for the custom title bar
                SetDragRegionForCustomTitleBar(_mainAppWindow);
            }
        }

        private bool TrySetCustomTitleBarColors()
        {
            if (AppWindowTitleBar.IsCustomizationSupported())
            {
                // Set Button colors to match the custom title bar
                _mainAppWindow.TitleBar.ButtonBackgroundColor = Colors.AliceBlue;
                _mainAppWindow.TitleBar.ButtonForegroundColor = Colors.Black;
                _mainAppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.LightGray;
                _mainAppWindow.TitleBar.ButtonInactiveForegroundColor = Colors.Gray;
                _mainAppWindow.TitleBar.ButtonHoverBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonHoverForegroundColor = Colors.White;
                _mainAppWindow.TitleBar.ButtonPressedBackgroundColor = Colors.Green;
                _mainAppWindow.TitleBar.ButtonPressedForegroundColor = Colors.White;

                // We're using a system drawn titlebar, so we want to set the colors of the actual titlebar area as well
                if (!_mainAppWindow.TitleBar.ExtendsContentIntoTitleBar)
                {
                    _mainAppWindow.TitleBar.ForegroundColor = Colors.Black;
                    _mainAppWindow.TitleBar.BackgroundColor = Colors.AliceBlue;
                    _mainAppWindow.TitleBar.InactiveBackgroundColor = Colors.LightGray;
                    _mainAppWindow.TitleBar.InactiveForegroundColor = Colors.Gray;
                }
                return (true);
            }
            else
            {
                return (false);
            }
        }

        private void titleBarBrandingSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    if (TrySetCustomTitleBarColors())
                    {
                        _mainAppWindow.Title = "Default titlebar with custom color customization";
                        // Make sure the custom titlebar XAML element is not visible
                        _mainPage.MyTitleBar.Visibility = Visibility.Collapsed;
                    }
                }
                else
                {
                    // We should show the default system titlebar state, reset the titlebar and update the title to say so.
                    ResetTitlebar();
                    _mainAppWindow.Title = "Default titlebar";
                }
            }
        }

        private void titleCustomSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggleSwitch = sender as ToggleSwitch;
            if (toggleSwitch != null)
            {
                if (toggleSwitch.IsOn == true)
                {
                    _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = true;

                    // Disable the color branding option for the titlebar so we don't collide in functionality.
                    titleBarBrandingSwitch.IsEnabled = false;

                    // We're having a custom drawn title bar, so enable tall height for the caption controls area
                    tallTitleBarSwitch.IsEnabled = true;

                    // Show the custom titlebar
                    _mainPage.MyTitleBar.Visibility = Visibility.Visible;

                    if (TrySetCustomTitleBarColors())
                    {
                        // Set the drag region for the custom title bar
                        SetDragRegionForCustomTitleBar(_mainAppWindow);
                    }
                    else
                    {
                        // Customization failed. This means we can't be extending content into titlebar either.
                        // Set the property back to false.
                        _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = true;
                    }
                }
                else
                {
                    _mainAppWindow.TitleBar.ExtendsContentIntoTitleBar = false;

                    // Bring back the default titlebar and disable the option to have a tall height for the caption controls area
                    _mainPage.MyTitleBar.Visibility = Visibility.Collapsed;
                    tallTitleBarSwitch.IsEnabled = false;

                    // reset the title bar to default state
                    ResetTitlebar();
                }
            }
        }
    }
}
