#pragma once

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include "MainWindow.g.h"

#pragma pop_macro("GetCurrentTime")

namespace winrt::WinUI3TemplateCpp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

    private:

        HWND _hwnd{ nullptr };

        void SetWindowSize(HWND hwnd, const int width, const int height);
        HWND GetWindowHandle();
        void LoadIcon(HWND hwnd, wchar_t* iconName);
        void ClipOrCenterRectToMonitorWin32(RECT* prc, bool UseWorkArea);
        void PlacementCenterWindowInMonitorWin32(HWND hwnd);
    };
}

namespace winrt::WinUI3TemplateCpp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
