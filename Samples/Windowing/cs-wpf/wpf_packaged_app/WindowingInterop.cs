﻿using System;
using System.Windows;
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
    internal class Interop
    {

        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowHandleFromWindowId", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowHandleFromWindowId(WindowId windowId, out IntPtr result);

        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowIdFromWindowHandle", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindowHandle(IntPtr hwnd, out WindowId windowId);
    }

    public static class AppWindowExtensions
    {
        public static Microsoft.UI.Windowing.AppWindow GetAppWindowFromWPFWindow(this Window wpfWindow)
        {
            // Get the HWND of the top level WPF window.
            var helper = new WindowInteropHelper(wpfWindow);
            IntPtr hwnd = (HwndSource.FromHwnd(helper.EnsureHandle())
                as IWin32Window).Handle;

            // Get the WindowId from the HWND.
            Microsoft.UI.WindowId windowId;
            Interop.GetWindowIdFromWindowHandle(hwnd, out windowId);
            
            // Get an AppWindow from the WindowId.
            Microsoft.UI.Windowing.AppWindow appWindow =
            Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);

            return appWindow;
        }
    }
}
