#pragma once
#include "MainWindow.g.h"

namespace winrt::CppWinUiDesktopInstancing::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void activationTrackerButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::CppWinUiDesktopInstancing::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
