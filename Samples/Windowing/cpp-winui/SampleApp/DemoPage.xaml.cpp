// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "DemoPage.xaml.h"
#if __has_include("DemoPage.g.cpp")
#include "DemoPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;


namespace winrt::SampleApp::implementation
{

    DemoPage::DemoPage()
    {
        InitializeComponent();
    }

    void DemoPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        Window window = e.Parameter().as<Window>();
        MainWindow mainWindow = window.as<MainWindow>();
        AppWindow appWindow = mainWindow.AppWindow();
        m_mainAppWindow = appWindow;
    }

    void DemoPage::TitleBtn_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        m_mainAppWindow.Title(TitleTextBox().Text());
    }

    void DemoPage::SizeBtn_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        int windowWidth = 0;
        int windowHeight = 0;

        windowWidth = _wtoi(WidthTextBox().Text().c_str());
        windowHeight = _wtoi(HeightTextBox().Text().c_str());

        if (windowHeight > 0 && windowWidth > 0)
        {
            winrt::Windows::Graphics::SizeInt32 windowSize;
            windowSize.Height = windowHeight;
            windowSize.Width = windowWidth;
            m_mainAppWindow.Resize(windowSize);
        }
    }

}
