// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Windows.UI;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using Color = Windows.UI.Color;
using Microsoft.UI.Xaml.Controls;
using System.Xml;
using System.Windows.Data;

namespace wpf_packaged_app
{
    public static class AppWindowExtensions
    {
        public static AppWindow GetAppWindowFromWPFWindow(this Window wpfWindow)
        {
            // Get the HWND of the top level WPF window.
            var hwnd = new WindowInteropHelper(wpfWindow).EnsureHandle();

            // Get the WindowId from the HWND.
            WindowId windowId = Win32Interop.GetWindowIdFromWindow(hwnd);

            // Return AppWindow from the WindowId.
            return AppWindow.GetFromWindowId(windowId);
        }
    }
}
