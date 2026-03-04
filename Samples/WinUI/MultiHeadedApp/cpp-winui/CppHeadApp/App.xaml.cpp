#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::CppHeadApp::implementation
{
    App::App()
    {
        InitializeComponent();
    }

    void App::OnLaunched([[maybe_unused]] Microsoft::UI::Xaml::LaunchActivatedEventArgs const& e)
    {
        m_window = make<MainWindow>();
        m_window.Activate();
    }
}
