#pragma once

#include "MainWindow.g.h"
#include <vector>

namespace winrt::FilePickersAppSinglePackaged::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        Windows::Foundation::IAsyncAction NewPickSingleFile_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction NewPickMultipleFiles_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction NewFileTypeChoices_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        Windows::Foundation::IAsyncAction NewPickFolder_Click(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

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
