// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MainWindow.g.h"
#include "pch.h"

namespace winrt::CppApp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

    private:
        HWND _hwnd{ nullptr };
        void SetWindowSize(HWND hwnd, const int width, const int height);
        HWND GetWindowHandle();
        void LoadIcon(HWND hwnd, wchar_t const* iconName);
        void ClipOrCenterRectToMonitorWin32(RECT& rc);
        void PlacementCenterWindowInMonitorWin32(HWND hwnd);
    };
}

namespace winrt::CppApp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
