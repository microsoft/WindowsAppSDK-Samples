// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "MainWindow.g.h"

namespace winrt::cpp_winui::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void MyButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void OnKeyDownHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void OnKeyUpHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void OnCharacterReceivedHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& e);
        void OnCtrlRInvoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& sender, Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs e);
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
