//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "MainWindow.g.h"
#include "pch.h"

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
