#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"

#include "microsoft.ui.xaml.window.h" 
#include "SampleConfiguration.h"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::WinUI3TemplateCpp::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        Title(winrt::WinUI3TemplateCpp::Settings::FeatureName);

        HWND hwnd = GetWindowHandle();

        LoadIcon(hwnd, L"windows-sdk.ico");
        SetWindowSize(hwnd, 1220, 1080);
        PlacementCenterWindowInMonitorWin32(hwnd);
    }

    HWND MainWindow::GetWindowHandle()
    {
        if (_hwnd == nullptr)
        {
            Window window = this->try_as<Window>();
            window.as<IWindowNative>()->get_WindowHandle(&_hwnd);
        }
        return _hwnd;
    }

    void MainWindow::LoadIcon(HWND hwnd, wchar_t* iconName)
    {
        auto hSmallIcon = LoadImage(NULL,
            iconName,
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CYSMICON),
            LR_LOADFROMFILE);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmallIcon);

        auto hBigIcon = LoadImage(
            NULL,
            iconName,
            IMAGE_ICON,
            GetSystemMetrics(SM_CXICON),
            GetSystemMetrics(SM_CYICON),
            LR_LOADFROMFILE);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBigIcon);
    }

    void MainWindow::SetWindowSize(const HWND hwnd, const int width, const int height)
    {
        // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
        auto dpi = GetDpiForWindow(hwnd);
        float scalingFactor = static_cast<float>(dpi / 96);
        auto widthScaled = static_cast<int>(width * scalingFactor);
        auto heightScaled = static_cast<int>(height * scalingFactor);

        SetWindowPos(hwnd, HWND_TOP, 0, 0, widthScaled, heightScaled, SWP_NOMOVE);
    }

    void MainWindow::PlacementCenterWindowInMonitorWin32(HWND hwnd)
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        ClipOrCenterRectToMonitorWin32(&rc, true);
        SetWindowPos(hwnd, HWND_TOP,
            rc.left, rc.top, 0, 0,
            SWP_NOSIZE |
            SWP_NOZORDER |
            SWP_NOACTIVATE);
    }

    void MainWindow::ClipOrCenterRectToMonitorWin32(RECT* prc, bool UseWorkArea)
    {
        HMONITOR hMonitor;
        RECT rc;
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;

        hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

        MONITORINFO mi;
        mi.cbSize = sizeof(MONITORINFO);

        GetMonitorInfo(hMonitor, &mi);

        rc = UseWorkArea ? mi.rcWork : mi.rcMonitor;
        prc->left = rc.left + (rc.right - rc.left - w) / 2;
        prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
        prc->right = prc->left + w;
        prc->bottom = prc->top + h;
    }
}
