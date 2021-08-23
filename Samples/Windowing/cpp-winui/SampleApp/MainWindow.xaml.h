#pragma once

#include "MainWindow.g.h"


namespace winrt::SampleApp::implementation
{

struct MainWindow : MainWindowT<MainWindow>
{
public:
    MainWindow();

    winrt::AppWindow AppWindow();

    void NavigationView_Loaded(winrt::IInspectable const& sender, winrt::RoutedEventArgs const& args);
    void NavigationView_ItemInvoked(winrt::IInspectable const& sender, winrt::NavigationViewItemInvokedEventArgs const& args);
    void NavigationView_BackRequested(winrt::NavigationView const& sender, winrt::NavigationViewBackRequestedEventArgs const& args);
    void MyWindowIcon_DoubleTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);

private:
    winrt::AppWindow GetAppWindowForCurrentWindow();

    winrt::AppWindow m_mainAppWindow{ nullptr };
    hstring m_windowTitle = L"WinUI Desktop C++ Sample App";
};

}


namespace winrt::SampleApp::factory_implementation
{

struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
{
};

}
