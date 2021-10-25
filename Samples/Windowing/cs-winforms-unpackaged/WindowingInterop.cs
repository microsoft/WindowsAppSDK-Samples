using System;
using System.Windows;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Windows.UI;
using Color = Windows.UI.Color;
using Microsoft.UI.Xaml.Controls;
using System.Xml;


namespace winforms_unpackaged_app
{
    public static class AppWindowExtensions
    {
        public static Microsoft.UI.Windowing.AppWindow GetAppWindowFromWinformsWindow(this IWin32Window winformsWindow)
        {
            // Get the HWND of the top level WinForms window
            IntPtr hwnd = winformsWindow.Handle;        
            
            // Get the WindowId from the HWND.
            Microsoft.UI.WindowId windowId;
            windowId = Win32Interop.GetWindowIdFromWindow(hwnd);

            // Get an AppWindow from the WindowId.
            Microsoft.UI.Windowing.AppWindow appWindow =
            Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);

            return appWindow;
        }
    }
}
