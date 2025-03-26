using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WinFormsWithIsland
{
    internal class WindowsAppSdkHelper
    {
        /// <summary>
        /// Call when your app starts to use the WindowsAppSDK UI stack (including WinUI3).
        /// Apps using the WindowsAppSDK UI stack are required to call ContentPreTranslateMessage.
        /// </summary>
        public static void EnableContentPreTranslateMessageInEventLoop()
        {
            Application.AddMessageFilter(new ContentPreTranslateMessageFilter());
        }

        class ContentPreTranslateMessageFilter : IMessageFilter
        {
            bool IMessageFilter.PreFilterMessage(ref Message m)
            {
                MSG msg = new MSG(m);
                return ContentPreTranslateMessage(ref msg);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MSG
        {
            public MSG(Message other)
            {
                hwnd = other.HWnd;
                message = (uint)other.Msg;
                wParam = other.WParam;
                lParam = other.LParam;
            }

            public IntPtr hwnd;
            public uint message;
            public IntPtr wParam;
            public IntPtr lParam;
            public int time = 0;
            public int pt_x = 0;
            public int pt_y = 0;
        }

        [DllImport("Microsoft.UI.Windowing.Core.dll")]
        public static extern bool ContentPreTranslateMessage(ref MSG message);
    }
}
