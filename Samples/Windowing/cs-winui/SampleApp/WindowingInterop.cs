using System;
using System.Runtime.InteropServices;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using WinRT;

namespace SampleApp
{
    internal class Interop
    {
        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowFromWindowId", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowFromWindowId(WindowId windowId, out IntPtr result);


        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowIdFromWindow", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindow(IntPtr hwnd, out WindowId result);
    }

    public static class AppWindowExtensions
    {
        public static AppWindow GetAppWindow(this Microsoft.UI.Xaml.Window window)
        {
            IntPtr windowHandle = WinRT.Interop.WindowNative.GetWindowHandle(window);

            return GetAppWindowFromWindowHandle(windowHandle);
        }

        private static AppWindow GetAppWindowFromWindowHandle(IntPtr windowHandle)
        {
            WindowId windowId;

            Interop.GetWindowIdFromWindow(windowHandle, out windowId);

            return AppWindow.GetFromWindowId(windowId);
        }

    }
}
