#pragma once

#include "MainWindow.g.h"

#include <vector>

namespace winrt::FilePickersAppSinglePackaged::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        winrt::fire_and_forget NewPickSingleFile_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::fire_and_forget NewPickMultipleFiles_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::fire_and_forget NewPickSaveFile_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::fire_and_forget NewPickFolder_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        void LogResult(winrt::hstring const& message);
        Microsoft::Windows::Storage::Pickers::PickerLocationId GetSelectedNewLocationId();
        Microsoft::Windows::Storage::Pickers::PickerViewMode GetSelectedNewViewMode();
        std::vector<winrt::hstring> GetFileFilters();
    };
}

namespace winrt::FilePickersAppSinglePackaged::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
