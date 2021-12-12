// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "MainWindow.g.h"

namespace winrt::PhotoEditor::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
    };
}

namespace winrt::PhotoEditor::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
