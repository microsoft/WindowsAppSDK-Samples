// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "MainWindow.g.h"

namespace winrt::CppWinUiDesktopInstancing::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void ActivationInfoButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender, 
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::CppWinUiDesktopInstancing::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
