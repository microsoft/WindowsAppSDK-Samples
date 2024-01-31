// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#include <winrt/Microsoft.UI.Interop.h>
#endif


using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;


namespace winrt::SampleApp::implementation
{

    MainWindow::MainWindow()
    {
        InitializeComponent();

        m_mainAppWindow = GetAppWindowForCurrentWindow();
        this->Title(m_windowTitle);
    }


    winrt::AppWindow MainWindow::MyAppWindow()
    {
        return m_mainAppWindow;
    }


    void MainWindow::NavigationView_Loaded(winrt::IInspectable const& /*sender*/, winrt::RoutedEventArgs const& /*args*/)
    {
        ContentFrame().Navigate(xaml_typename<winrt::SampleApp::DemoPage>(), *this);
    }


    void MainWindow::NavigationView_ItemInvoked(winrt::IInspectable const& /*sender*/, winrt::NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.InvokedItemContainer() != nullptr)
        {
            IInspectable tag = args.InvokedItemContainer().Tag();
            hstring tagValue = unbox_value<hstring>(tag);

            if (tagValue == L"basics")
            {
                ContentFrame().Navigate(xaml_typename<SampleApp::DemoPage>(), *this);
            }
            else if (tagValue == L"presenters")
            {
                ContentFrame().Navigate(xaml_typename<SampleApp::PresenterPage>(), *this);
            }
            else if (tagValue == L"configs")
            {
                ContentFrame().Navigate(xaml_typename<winrt::SampleApp::ConfigPage>(), *this);
            }
            else if (tagValue == L"titlebar")
            {
                ContentFrame().Navigate(xaml_typename<winrt::SampleApp::TitlebarPage>(), *this);
            }
        }
    }


    void MainWindow::NavigationView_BackRequested(winrt::NavigationView const& /*sender*/, winrt::NavigationViewBackRequestedEventArgs const& /*args*/)
    {
        if (ContentFrame().CanGoBack())
        {
            ContentFrame().GoBack();
        }
    }

    void MainWindow::MyWindowIcon_DoubleTapped(winrt::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& /*e*/)
    {
        this->MyAppWindow().Destroy();
    }

    winrt::AppWindow MainWindow::GetAppWindowForCurrentWindow()
    {
        // Get access to IWindowNative
        winrt::SampleApp::MainWindow thisWindow = *this;
        winrt::com_ptr<IWindowNative> windowNative = thisWindow.as<IWindowNative>();

        //Get the HWND for the XAML Window
        HWND hWnd;
        windowNative->get_WindowHandle(&hWnd);

        // Get the WindowId for our window
        winrt::WindowId windowId;
        windowId = winrt::GetWindowIdFromWindow(hWnd);

        // Get the AppWindow for the WindowId
        Microsoft::UI::Windowing::AppWindow appWindow = Microsoft::UI::Windowing::AppWindow::GetFromWindowId(windowId);

        return appWindow;
    }
}
