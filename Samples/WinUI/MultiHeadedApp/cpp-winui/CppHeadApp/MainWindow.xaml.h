#pragma once
#include "MainWindow.g.h"

namespace winrt::CppHeadApp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow() { InitializeComponent(); }
    };
}

namespace winrt::CppHeadApp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
