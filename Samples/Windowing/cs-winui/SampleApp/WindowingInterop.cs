using System;
using System.Runtime.InteropServices;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using WinRT;

namespace SampleApp
{
    internal class Interop
    {
        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowHandleFromWindowId", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowHandleFromWindowId(WindowId windowId, out IntPtr result);


        [DllImport("Microsoft.Internal.FrameworkUdk.dll", EntryPoint = "Windowing_GetWindowIdFromWindowHandle", CharSet = CharSet.Unicode)]
        public static extern IntPtr GetWindowIdFromWindowHandle(IntPtr hwnd, out WindowId result);
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

            Interop.GetWindowIdFromWindowHandle(windowHandle, out windowId);

            return AppWindow.GetFromWindowId(windowId);
        }
    }
}
