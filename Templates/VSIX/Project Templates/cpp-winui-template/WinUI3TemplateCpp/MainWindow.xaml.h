﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MainWindow.g.h"
#include "pch.h"

namespace winrt::$safeprojectname$::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

    private:
        HWND _hwnd{ nullptr };
        void SetWindowSize(HWND hwnd, const int width, const int height);
        HWND GetWindowHandle();
        void LoadIcon(HWND hwnd, wchar_t* iconName);
        void ClipOrCenterRectToMonitorWin32(RECT* prc);
        void PlacementCenterWindowInMonitorWin32(HWND hwnd);
    };
}

namespace winrt::$safeprojectname$::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
