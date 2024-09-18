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

        /// <summary>
        /// Call to move focus to the next focusable control in the parent.
        /// </summary>
        /// <param name="parent">Control or Form that contains the controls.</param>
        /// <param name="start">Control we're starting with.</param>
        /// <param name="forward">If true, focus is moving forward.  If not, backward.</param>
        public static void FocusNextFocusableControl(Control parent, Control start, bool forward)
        {
            // GetNextControl can return controls that aren't tab stops, so keep going until we find one that is.
            System.Windows.Forms.Control next = start;
            do
            {
                next = parent.GetNextControl(next, forward);
            }
            while (next != null && next.TabStop == false);

            if (next == null)
            {
                // Oops, we ran out of controls.  Get the first control in the parent.
                next = parent.GetNextControl(null, forward);
            }

            if (next != null)
            {
                next.Focus();
            }
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
