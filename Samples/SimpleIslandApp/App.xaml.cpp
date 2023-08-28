#include "pch.h"

#include "App.xaml.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::SimpleIslandApp::implementation
{
    App::App()
    {
        m_windowsXamlManager = winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
    }

}

