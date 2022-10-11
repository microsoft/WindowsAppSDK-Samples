// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "TitlebarPage.xaml.h"
#if __has_include("TitlebarPage.g.cpp")
#include "TitlebarPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;


namespace winrt::SampleApp::implementation
{
    winrt::hstring defaultTitle = L"WinUI C++ Desktop sample App";

    TitlebarPage::TitlebarPage()
    {
        InitializeComponent();
    }

    void TitlebarPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        m_mainWindow = e.Parameter().as<MainWindow>();
        AppWindow appWindow = m_mainWindow.MyAppWindow();
        m_appWindow = appWindow;
        m_changedToken = m_appWindow.Changed({ this, &TitlebarPage::AppWindowChangedHandler });
    }

    void TitlebarPage::AppWindowChangedHandler(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const& e)
    {
        if (e.DidSizeChange() && sender.as<AppWindow>().TitleBar().ExtendsContentIntoTitleBar())
        {
            SetCustomTitleBarDragRegion();
        }
    }

    void TitlebarPage::TitlebarBrandingBtn_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        m_appWindow.TitleBar().ResetToDefault();
        m_mainWindow.MyTitleBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

        m_brandTitleBar = !m_brandTitleBar;
        // Check to see if customization is supported. Currently only supported on Windows 11.
        if (AppWindowTitleBar::IsCustomizationSupported() && m_brandTitleBar)
        {
            m_appWindow.Title(L"Default titlebar with custom color customization");
            m_appWindow.TitleBar().ForegroundColor(Colors::White());
            m_appWindow.TitleBar().BackgroundColor(Colors::DarkOrange());
            m_appWindow.TitleBar().InactiveBackgroundColor(Colors::Blue());
            m_appWindow.TitleBar().InactiveForegroundColor(Colors::White());

            //Buttons
            m_appWindow.TitleBar().ButtonBackgroundColor(Colors::DarkOrange());
            m_appWindow.TitleBar().ButtonForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Blue());
            m_appWindow.TitleBar().ButtonInactiveForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonHoverBackgroundColor(Colors::Green());
            m_appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::DarkOrange());
            m_appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());
        }
        else
        {
            m_appWindow.TitleBar().ResetToDefault();
            m_appWindow.Title(defaultTitle);
        }
    }

    void TitlebarPage::TitlebarCustomBtn_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        // Check to see if customization is supported. Currently only supported on Windows 11.
        if (AppWindowTitleBar::IsCustomizationSupported() && !m_customTitleBar) {
            m_customTitleBar = true;
            m_appWindow.TitleBar().ExtendsContentIntoTitleBar(true);

            // Show the custom titlebar
            m_mainWindow.MyTitleBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);

            // Set Button colors to match the custom titlebar
            m_appWindow.TitleBar().ButtonBackgroundColor(Colors::Blue());
            m_appWindow.TitleBar().ButtonForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Blue());
            m_appWindow.TitleBar().ButtonInactiveForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonHoverBackgroundColor(Colors::Green());
            m_appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::Green());
            m_appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());

            //Set the drag region for the custom titlebar
            SetCustomTitleBarDragRegion();
        }
        else
        {
            // Bring back the default titlebar
            m_customTitleBar = false;
            m_mainWindow.MyTitleBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            m_appWindow.TitleBar().ResetToDefault();
        }
    }

    void TitlebarPage::ResetTitlebarBtn_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        m_mainWindow.MyTitleBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        m_appWindow.TitleBar().ResetToDefault();
        m_appWindow.Title(defaultTitle);
    }

    void TitlebarPage::SetCustomTitleBarDragRegion()
    {
        // Get the titlebar heigt and set our XAML titlebar element to match
        int titleBarHeight = m_appWindow.TitleBar().Height();
        m_mainWindow.MyTitleBar().Height(titleBarHeight);

        // Get caption button occlusion information
        // Use LeftInset if you've explicitly set your window layout to RTL or if app language is a RTL language
        double CaptionButtonOcclusionWidth = m_appWindow.TitleBar().RightInset();

        // Define your drag Regions
        int windowIconWidthAndPadding = static_cast<int>(m_mainWindow.MyWindowIcon().ActualWidth() + static_cast<int>(m_mainWindow.MyWindowIcon().Margin().Right));
        int dragRegionWidth = static_cast<int>(m_appWindow.Size().Width - (CaptionButtonOcclusionWidth + windowIconWidthAndPadding));

        std::vector<Windows::Graphics::RectInt32> dragRects;
        Windows::Graphics::RectInt32 dragRect;

        dragRect.X = windowIconWidthAndPadding;
        dragRect.Y = 0;
        dragRect.Height = titleBarHeight;
        dragRect.Width = dragRegionWidth;

        dragRects.push_back(dragRect);
        m_appWindow.TitleBar().SetDragRectangles(dragRects);
    }
}
