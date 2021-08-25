using Microsoft.UI.Xaml;
using System;
using System.Runtime.InteropServices;

namespace WinUI3TemplateCs
{
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            Title = Settings.FeatureName;

            IntPtr hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            LoadIcon(hwnd,"Assets/windows-sdk.ico");
            //SetWindowSize(hwnd, 1220, 1080);
            //PlacementCenterWindowInMonitorWin32(hwnd);
        }

        private void LoadIcon(IntPtr hwnd, string iconName)
        {
            const int ICON_SMALL = 0;
            const int ICON_BIG = 1;

            var hSmallIcon = PInvoke.User32.LoadImage(
                IntPtr.Zero,
                iconName,
                PInvoke.User32.ImageType.IMAGE_ICON,
                PInvoke.User32.GetSystemMetrics(PInvoke.User32.SystemMetric.SM_CXSMICON),
                PInvoke.User32.GetSystemMetrics(PInvoke.User32.SystemMetric.SM_CYSMICON),
                PInvoke.User32.LoadImageFlags.LR_LOADFROMFILE);

            PInvoke.User32.SendMessage(hwnd, PInvoke.User32.WindowMessage.WM_SETICON, (IntPtr)ICON_SMALL, hSmallIcon);

            var hBigIcon = PInvoke.User32.LoadImage(
                IntPtr.Zero,
                iconName,
                PInvoke.User32.ImageType.IMAGE_ICON,
                PInvoke.User32.GetSystemMetrics(PInvoke.User32.SystemMetric.SM_CXICON),
                PInvoke.User32.GetSystemMetrics(PInvoke.User32.SystemMetric.SM_CYICON),
                PInvoke.User32.LoadImageFlags.LR_LOADFROMFILE);

            PInvoke.User32.SendMessage(hwnd, PInvoke.User32.WindowMessage.WM_SETICON, (IntPtr)ICON_BIG, hBigIcon);
        }
        private void SetWindowSize(IntPtr hwnd, int width, int height)
        {
            // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
            var dpi = PInvoke.User32.GetDpiForWindow(hwnd);
            float scalingFactor = (float)dpi / 96;
            width = (int)(width * scalingFactor);
            height = (int)(height * scalingFactor);

            PInvoke.User32.SetWindowPos(hwnd, PInvoke.User32.SpecialWindowHandles.HWND_TOP,
                                        0, 0, width, height,
                                        PInvoke.User32.SetWindowPosFlags.SWP_NOMOVE);
        }

        private void PlacementCenterWindowInMonitorWin32(IntPtr hwnd)
        {
            PInvoke.RECT rc;
            PInvoke.User32.GetWindowRect(hwnd, out rc);
            ClipOrCenterRectToMonitorWin32(ref rc, true, true);
            PInvoke.User32.SetWindowPos(hwnd, PInvoke.User32.SpecialWindowHandles.HWND_TOP,
                rc.left, rc.top, 0, 0,
                PInvoke.User32.SetWindowPosFlags.SWP_NOSIZE |
                PInvoke.User32.SetWindowPosFlags.SWP_NOZORDER |
                PInvoke.User32.SetWindowPosFlags.SWP_NOACTIVATE);
        }

        private void ClipOrCenterRectToMonitorWin32(ref PInvoke.RECT prc, bool UseWorkArea, bool IsCenter)
        {
            IntPtr hMonitor;
            PInvoke.RECT rc;
            int w = prc.right - prc.left;
            int h = prc.bottom - prc.top;

            hMonitor = PInvoke.User32.MonitorFromRect(ref prc, PInvoke.User32.MonitorOptions.MONITOR_DEFAULTTONEAREST);

            PInvoke.User32.MONITORINFO mi = new PInvoke.User32.MONITORINFO();
            mi.cbSize = Marshal.SizeOf(mi);

            PInvoke.User32.GetMonitorInfo(hMonitor, ref mi);

            rc = UseWorkArea ? mi.rcWork : mi.rcMonitor;

            if (IsCenter)
            {
                prc.left = rc.left + (rc.right - rc.left - w) / 2;
                prc.top = rc.top + (rc.bottom - rc.top - h) / 2;
                prc.right = prc.left + w;
                prc.bottom = prc.top + h;
            }
            else
            {
                prc.left = Math.Max(rc.left, Math.Min(rc.right - w, prc.left));
                prc.top = Math.Max(rc.top, Math.Min(rc.bottom - h, prc.top));
                prc.right = prc.left + w;
                prc.bottom = prc.top + h;
            }
        }

    }
}
