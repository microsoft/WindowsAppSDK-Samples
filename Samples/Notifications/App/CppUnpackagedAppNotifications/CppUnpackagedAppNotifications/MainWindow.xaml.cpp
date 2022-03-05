// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"

#include "microsoft.ui.xaml.window.h" 
#include "SampleConfiguration.h"
#endif
#include <MddBootstrap.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::AppNotifications;
}

HRESULT LoadWindowsAppSDK()
{
#if 0
    // Major.Minor version, MinVersion=0 to find any framework package for this major.minor version
    const UINT32 majorMinorVersion{ 0x00010001 }; //  { Major << 16) | Minor };
    PCWSTR versionTag{ L"" };
    const PACKAGE_VERSION minVersion{};
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"\nError 0x%08X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }
#endif
    return S_OK;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainWindow::MainWindow()
    {
        if (SUCCEEDED(::LoadWindowsAppSDK()))
        {
            winrt::AppNotificationManager::Default().Register();
            //infoBar().Message(L"Could not load WindowsAppSDK", InfoBarSeverity::Error);
        }

        InitializeComponent();

        Title(winrt::CppUnpackagedAppNotifications::SampleConfig::FeatureName);

        HWND hwnd = GetWindowHandle();
        LoadIcon(hwnd, L"Assets/windows-sdk.ico");
        SetWindowSize(hwnd, 1050, 800);
        PlacementCenterWindowInMonitorWin32(hwnd);
    }

    HWND MainWindow::GetWindowHandle()
    {
        if (_hwnd == nullptr)
        {
            Window window = *this;
            window.as<IWindowNative>()->get_WindowHandle(&_hwnd);
        }
        return _hwnd;
    }

    void MainWindow::LoadIcon(HWND hwnd, wchar_t const* iconPath)
    {
        HANDLE hSmallIcon = LoadImageW(nullptr, iconPath, IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
            LR_LOADFROMFILE | LR_SHARED);
        SendMessageW(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hSmallIcon));

        HANDLE hBigIcon = LoadImageW(nullptr, iconPath, IMAGE_ICON,
            GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
            LR_LOADFROMFILE | LR_SHARED);
        SendMessageW(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hBigIcon));
    }

    void MainWindow::SetWindowSize(HWND hwnd, int width, int height)
    {
        // Win32 uses pixels and WinUI 3 uses effective pixels, so you should apply the DPI scale factor
        const UINT dpi = GetDpiForWindow(hwnd);
        const float scalingFactor = static_cast<float>(dpi) / 96;
        const int widthScaled = static_cast<int>(width * scalingFactor);
        const int heightScaled = static_cast<int>(height * scalingFactor);

        SetWindowPos(hwnd, nullptr, 0, 0, widthScaled, heightScaled, SWP_NOMOVE | SWP_NOZORDER);
    }

    void MainWindow::PlacementCenterWindowInMonitorWin32(HWND hwnd)
    {
        RECT windowMontiorRectToAdjust;
        GetWindowRect(hwnd, &windowMontiorRectToAdjust);
        ClipOrCenterRectToMonitorWin32(windowMontiorRectToAdjust);
        SetWindowPos(hwnd, nullptr, windowMontiorRectToAdjust.left,
            windowMontiorRectToAdjust.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void MainWindow::ClipOrCenterRectToMonitorWin32(RECT& adjustedWindowRect)
    {
        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(MonitorFromRect(&adjustedWindowRect, MONITOR_DEFAULTTONEAREST), &mi);

        const auto& rcWork = mi.rcWork;
        const int w = adjustedWindowRect.right - adjustedWindowRect.left;
        const int h = adjustedWindowRect.bottom - adjustedWindowRect.top;

        adjustedWindowRect.left = rcWork.left + (rcWork.right - rcWork.left - w) / 2;
        adjustedWindowRect.top = rcWork.top + (rcWork.bottom - rcWork.top - h) / 2;
        adjustedWindowRect.right = adjustedWindowRect.left + w;
        adjustedWindowRect.bottom = adjustedWindowRect.top + h;
    }
}
