#pragma once
#include "MainWindow.g.h"

namespace winrt::CppSecondaryApp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow() { InitializeComponent(); }
    };
}

namespace winrt::CppSecondaryApp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
