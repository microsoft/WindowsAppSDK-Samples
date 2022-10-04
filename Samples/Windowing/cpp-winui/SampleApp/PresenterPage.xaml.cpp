// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "PresenterPage.xaml.h"
#if __has_include("PresenterPage.g.cpp")
#include "PresenterPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;


namespace winrt::SampleApp::implementation
{
    PresenterPage::PresenterPage()
    {
        InitializeComponent();
    }

    void PresenterPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        Window window = e.Parameter().as<Window>();
        MainWindow mainWindow = window.as<MainWindow>();
        m_mainAppWindow = mainWindow.MyAppWindow();
    }

    void PresenterPage::SwitchPresenter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        std::string buttonName = to_string(sender.as<Button>().Name());
        AppWindowPresenterKind newPresenterKind = AppWindowPresenterKind::Default;

        if (buttonName == "CompactoverlaytBtn")
        {
            newPresenterKind = AppWindowPresenterKind::CompactOverlay;
        }
        else if (buttonName == "FullscreenBtn")
        {
            newPresenterKind = AppWindowPresenterKind::FullScreen;
        }
        else if (buttonName == "OverlappedBtn")
        {
            newPresenterKind = AppWindowPresenterKind::Default;
        }

        if (newPresenterKind == m_mainAppWindow.Presenter().Kind())
        {
            m_mainAppWindow.SetPresenter(AppWindowPresenterKind::Default);
        }
        else
        {
            m_mainAppWindow.SetPresenter(newPresenterKind);
        }
    }
}
