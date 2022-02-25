#pragma once

#include "MainWindow.g.h"

namespace winrt::CustomEditControlWinAppSDK::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        winrt::Microsoft::UI::WindowId GetAppWindow();

    };
}

namespace winrt::CustomEditControlWinAppSDK::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
