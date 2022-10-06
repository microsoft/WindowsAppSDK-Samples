#pragma once

#include "MainWindow.g.h"

namespace winrt::WinUI3App::implementation
{
  struct MainWindow : MainWindowT<MainWindow>
  {
    MainWindow();

    int32_t MyProperty();
    void MyProperty(int32_t value);
    void BrowseForFolder(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    void BrowseForFile(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    fire_and_forget  PickFolder();
    fire_and_forget  PickFile();
    HWND GetWindowHandle();
  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
  {
  };
}
