using System;
using System.Runtime.InteropServices;
using System.Windows.Interop;

using Microsoft.UI;
using Microsoft.UI.Composition;
using Microsoft.UI.Content;

// Demo4_Step1_IslandHost
namespace CalculatorDemo
{
    internal class WpfIslandHost : HwndHost
    {
        public WpfIslandHost(Compositor compositor)
        {
            _compositor = compositor;
        }

        public DesktopChildSiteBridge DesktopChildSiteBridge { get; private set; }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            DesktopChildSiteBridge = Microsoft.UI.Content.DesktopChildSiteBridge.Create(
                _compositor,
                new Microsoft.UI.WindowId((ulong)hwndParent.Handle));

            // Mark as a layered window to handle transparent HWND with WPF.
            var hwndChild = Win32Interop.GetWindowFromWindowId(DesktopChildSiteBridge.WindowId);
            int prevExStyleFlags = GetWindowLong(hwndChild, GWL_EXSTYLE);
            SetWindowLongPtr(
                hwndChild,
                GWL_EXSTYLE,
                new IntPtr(prevExStyleFlags | WS_EX_LAYERED));

            return new HandleRef(null, (nint)DesktopChildSiteBridge.WindowId.Value);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DesktopChildSiteBridge.Dispose();
            DesktopChildSiteBridge = null;
        }

        Microsoft.UI.Composition.Compositor _compositor;

#region P/Invokes

        const int GWL_EXSTYLE = -20;
        const int WS_EX_LAYERED = 0x00080000;

        [DllImport("user32.dll")]
        static extern int GetWindowLong(
            IntPtr hwnd,
            int index);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetWindowLongPtr(
            IntPtr hWnd,
            int nIndex,
            IntPtr dwNewLong);

#endregion
    }
}
