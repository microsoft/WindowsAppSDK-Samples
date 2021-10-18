﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using System;
using System.Runtime.InteropServices;
using Windows.Win32.Foundation;
using Windows.Win32.Graphics.Gdi;
using Windows.Win32.UI.Controls;
using Windows.Win32.UI.WindowsAndMessaging;

using static Windows.Win32.PInvoke;
using static Windows.Win32.Constants;

namespace $safeprojectname$
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            Title = SampleConfig.FeatureName;

            HWND hwnd = (HWND)WinRT.Interop.WindowNative.GetWindowHandle(this);
            LoadIcon(hwnd,"Assets/windows-sdk.ico");
            SetWindowSize(hwnd, 1050, 800);
            PlacementCenterWindowInMonitorWin32(hwnd);
        }

        private unsafe void LoadIcon(HWND hwnd, string iconName)
        {
            const int ICON_SMALL = 0;
            const int ICON_BIG = 1;

            fixed (char* nameLocal = iconName)
            {
                HANDLE hSmallIcon = LoadImage(default,
                    nameLocal,
                    GDI_IMAGE_TYPE.IMAGE_ICON,
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CXSMICON),
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CYSMICON),
                    IMAGE_FLAGS.LR_LOADFROMFILE | IMAGE_FLAGS.LR_SHARED);
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, hSmallIcon.Value);
            }

            fixed (char* nameLocal = iconName)
            {
                HANDLE hBigIcon = LoadImage(default,
                    nameLocal,
                    GDI_IMAGE_TYPE.IMAGE_ICON,
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CXSMICON),
                    GetSystemMetrics(SYSTEM_METRICS_INDEX.SM_CYSMICON),
                    IMAGE_FLAGS.LR_LOADFROMFILE | IMAGE_FLAGS.LR_SHARED);
                SendMessage(hwnd, WM_SETICON, ICON_BIG, hBigIcon.Value);
            }
        }

        private void SetWindowSize(HWND hwnd, int width, int height)
        {
            // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
            uint dpi = GetDpiForWindow(hwnd);
            float scalingFactor = (float)dpi / 96;
            width = (int)(width * scalingFactor);
            height = (int)(height * scalingFactor);

            SetWindowPos(hwnd, default, 0, 0, width, height, SET_WINDOW_POS_FLAGS.SWP_NOMOVE | SET_WINDOW_POS_FLAGS.SWP_NOZORDER);
        }

        private void PlacementCenterWindowInMonitorWin32(HWND hwnd)
        {
            RECT rc;
            GetWindowRect(hwnd, out rc);
            ClipOrCenterRectToMonitorWin32(ref rc); 
            SetWindowPos(hwnd, default, rc.left, rc.top, 0, 0,
                         SET_WINDOW_POS_FLAGS.SWP_NOSIZE | 
                         SET_WINDOW_POS_FLAGS.SWP_NOZORDER | 
                         SET_WINDOW_POS_FLAGS.SWP_NOACTIVATE);     
        }

        private void ClipOrCenterRectToMonitorWin32(ref RECT prc)
        {
            MONITORINFO mi = new MONITORINFO(); 
            mi.cbSize = (uint)Marshal.SizeOf<MONITORINFO>();
            GetMonitorInfo(MonitorFromRect(prc, MONITOR_FROM_FLAGS.MONITOR_DEFAULTTONEAREST), ref mi);

            RECT rc = mi.rcWork;
            int w = prc.right - prc.left;
            int h = prc.bottom - prc.top;

            prc.left = rc.left + (rc.right - rc.left - w) / 2;
            prc.top = rc.top + (rc.bottom - rc.top - h) / 2;
            prc.right = prc.left + w;
            prc.bottom = prc.top + h;
        }
    }
}
