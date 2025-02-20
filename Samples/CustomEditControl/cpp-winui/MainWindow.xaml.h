// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::CustomEditControlCpp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void PointerPressed(const winrt::Windows::Foundation::IInspectable& /*sender*/, const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& /*args*/);
        winrt::Microsoft::UI::WindowId GetAppWindowId();
    };
}

namespace winrt::CustomEditControlCpp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
