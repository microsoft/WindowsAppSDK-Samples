// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ConfigPage.xaml.h"
#if __has_include("ConfigPage.g.cpp")
#include "ConfigPage.g.cpp"
#include <stddef.h>
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;


namespace winrt::SampleApp::implementation
{
    ConfigPage::ConfigPage()
    {
        InitializeComponent();
    }

    void ConfigPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        Window window = e.Parameter().as<Window>();
        MainWindow mainWindow = window.as<MainWindow>();
        m_mainAppWindow = mainWindow.MyAppWindow();

        // Disable the switches that control properties only available when Overlapped if we're in any other Presenter state
        if (m_mainAppWindow.Presenter().Kind() != AppWindowPresenterKind::Overlapped)
        {
            FrameToggle().IsEnabled(false);
            TitleBarToggle().IsEnabled(false);
            AlwaysOnTopToggle().IsEnabled(false);
            MaxToggle().IsEnabled(false);
            MinToggle().IsEnabled(false);
            ResizableToggle().IsEnabled(false);
        }
        else
        {
            FrameToggle().IsEnabled(true);
            TitleBarToggle().IsEnabled(true);
            AlwaysOnTopToggle().IsEnabled(true);
            MaxToggle().IsEnabled(true);
            MinToggle().IsEnabled(true);
            ResizableToggle().IsEnabled(true);
        }
    }

    void ConfigPage::ChangeWindowStyle(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        if (m_mainAppWindow)
        {
            OverlappedPresenter customOverlappedPresenter(NULL);
            std::string buttonName = to_string(sender.as<Button>().Name());

            if (buttonName == "MainWindowBtn")
            {
                customOverlappedPresenter = OverlappedPresenter::Create();
            }
            else if (buttonName == "ContextMenuBtn")
            {
                customOverlappedPresenter = OverlappedPresenter::CreateForContextMenu();
            }
            else if (buttonName == "DialogWindowBtn")
            {
                customOverlappedPresenter = OverlappedPresenter::CreateForDialog();
            }
            else if (buttonName == "ToolWindowBtn")
            {
                customOverlappedPresenter = OverlappedPresenter::CreateForToolWindow();
            }
            m_mainAppWindow.SetPresenter(customOverlappedPresenter);
        }

    }

    void ConfigPage::ChangeConfiguration(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        if (m_mainAppWindow)
        {
            ToggleSwitch toggleSwitch = sender.as<ToggleSwitch>();

            std::string toggleName = to_string(toggleSwitch.Name());
            OverlappedPresenter overlappedPresenter(NULL);
            if (m_mainAppWindow.Presenter().Kind() == AppWindowPresenterKind::Overlapped)
            {
                overlappedPresenter = m_mainAppWindow.Presenter().as<OverlappedPresenter>();
            }

            // Change the property that corresponts to the switch that was toggled
            if (toggleName == "FrameToggle")
            {
                overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder(), !overlappedPresenter.HasTitleBar());
            }
            else if (toggleName == "TitleBarToggle")
            {
                overlappedPresenter.SetBorderAndTitleBar(!overlappedPresenter.HasBorder(), !overlappedPresenter.HasTitleBar());
            }
            else if (toggleName == "MaxToggle")
            {
                overlappedPresenter.IsMaximizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "MinToggle")
            {
                overlappedPresenter.IsMinimizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "AlwaysOnTopToggle")
            {
                overlappedPresenter.IsAlwaysOnTop(toggleSwitch.IsOn());
            }
            else if (toggleName == "ResizableToggle")
            {
                overlappedPresenter.IsResizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "InUxToggle")
            {
                m_mainAppWindow.IsShownInSwitchers(toggleSwitch.IsOn());
            }
        
        }
    }
}



