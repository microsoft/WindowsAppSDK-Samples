using Microsoft.UI.Xaml;
using System;
using System.Runtime.InteropServices;
using WinRT;
using Microsoft.Win32.SafeHandles;
using Windows.Win32.Foundation;
using Windows.Win32.UI.Controls;
using Windows.Win32.UI.WindowsAndMessaging;
using Windows.Win32.Graphics.Gdi;
using static Windows.Win32.PInvoke;
using static Windows.Win32.Constants;

namespace WinUI3TemplateCs
{
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            Title = Settings.FeatureName;

            HWND hwnd = (HWND)WinRT.Interop.WindowNative.GetWindowHandle(this);
            LoadIcon(hwnd,"Assets/windows-sdk.ico");
            SetWindowSize(hwnd, 1220, 1080);
            PlacementCenterWindowInMonitorWin32(hwnd);
        }

        private unsafe void LoadIcon(HWND hwnd, string iconName)
        {
            const int ICON_SMALL = 0;
            const int ICON_BIG = 1;

            fixed (char* nameLocal = iconName)
            {
                HINSTANCE hInstLocal = default(HINSTANCE);
                HANDLE __result = LoadImage(hInstLocal, nameLocal,
                    GDI_IMAGE_TYPE.IMAGE_ICON,
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CXSMICON),
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CYSMICON),
                    IMAGE_FLAGS.LR_LOADFROMFILE);
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, __result.Value);
            }

            fixed (char* nameLocal = iconName)
            {
                HINSTANCE hInstLocal = default(HINSTANCE);
                HANDLE __result = LoadImage(hInstLocal, nameLocal,
                    GDI_IMAGE_TYPE.IMAGE_ICON,
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CXSMICON),
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CYSMICON),
                    IMAGE_FLAGS.LR_LOADFROMFILE);
                SendMessage(hwnd, WM_SETICON, ICON_BIG, __result.Value);
            }
        }

        private void SetWindowSize(HWND hwnd, int width, int height)
        {
            // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
            var dpi = GetDpiForWindow(hwnd);
            float scalingFactor = (float)(dpi / 96);
            width = (int)(width * scalingFactor);
            height = (int)(height * scalingFactor);

            SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SET_WINDOW_POS_FLAGS.SWP_NOMOVE);
        }

        private void PlacementCenterWindowInMonitorWin32(HWND hwnd)
        {
            RECT rc;
            GetWindowRect(hwnd, out rc);
            ClipOrCenterRectToMonitorWin32(rc, true, true);
            SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top, 0, 0,
                         SET_WINDOW_POS_FLAGS.SWP_NOSIZE | 
                         SET_WINDOW_POS_FLAGS.SWP_NOZORDER | 
                         SET_WINDOW_POS_FLAGS.SWP_NOACTIVATE);     
        }

        private void ClipOrCenterRectToMonitorWin32(RECT prc, bool UseWorkArea, bool IsCenter)
        {
            HMONITOR hMonitor;
            RECT rc;
            int w = prc.right - prc.left;
            int h = prc.bottom - prc.top;

            hMonitor = MonitorFromRect(prc, MONITOR_FROM_FLAGS.MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = new MONITORINFO();
            mi.cbSize = (uint)Marshal.SizeOf(mi);

            GetMonitorInfo(hMonitor, ref mi);

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
