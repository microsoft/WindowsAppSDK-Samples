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
        m_mainAppWindow = mainWindow.AppWindow();

        // Disable the switches that control properties only available when Overlapped if we're in any other Presenter state
        if (m_mainAppWindow.Presenter().Kind() != AppWindowPresenterKind::Overlapped)
        {
            BorderToggle().IsEnabled(false);
            TitleBarToggle().IsEnabled(false);
            AlwaysOnTopToggle().IsEnabled(false);
            MaxToggle().IsEnabled(false);
            MinToggle().IsEnabled(false);
            ResizableToggle().IsEnabled(false);
        }
        else
        {
            BorderToggle().IsEnabled(true);
            TitleBarToggle().IsEnabled(true);
            AlwaysOnTopToggle().IsEnabled(true);
            MaxToggle().IsEnabled(true);
            MinToggle().IsEnabled(true);
            ResizableToggle().IsEnabled(true);
        }
    }

    void ConfigPage::ChangeWindowStyle(winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        std::string buttonName = to_string(sender.as<Button>().Name());
        OverlappedPresenter customOverlappedPresenter = nullptr;
        
        if (buttonName == "MainWindowBtn")
        {
            customOverlappedPresenter = winrt::OverlappedPresenter::Create();
        }
        else
        {
            customOverlappedPresenter = winrt::OverlappedPresenter::CreateForContextMenu();
        }
        else if (buttonName == "DialogWindowBtn")
        {
            customOverlappedPresenter = winrt::OverlappedPresenter::CreateForDialog();
        }
        else if (buttonName == "ToolWindowBtn")
        {
            customOverlappedPresenter = winrt::OverlappedPresenter::CreateForToolWindow();
        }
        m_mainAppWindow.SetPresenter(customOverlappedPresenter);
    }

    void ConfigPage::ChangeConfiguration(winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_mainAppWindow)
        {
            ToggleSwitch toggleSwitch = sender.as<ToggleSwitch>();
            
            std::string toggleName = to_string(toggleSwitch.Name());
            OverlappedPresenter overlappedPresenter = m_mainAppWindow.Presenter().try_as<OverlappedPresenter>();

            // Change the property that corresponts to the switch that was toggled
            if (toggleName == "BorderToggle")
            {
                // In order to turn off the Border, you need to turn off the TitleBar too.
                if (!BorderToggle().IsOn() && TitleBarToggle().IsOn())
                {
                    TitleBarToggle().IsOn(false);
                }
                overlappedPresenter.SetBorderAndTitleBar(BorderToggle().IsOn(), TitleBarToggle().IsOn());
            }
            else if (toggleName == "TitleBarToggle")
            {
                // In order for TitleBar to be turned on, the Border has to be turn on too.
                if (!BorderToggle().IsOn() && TitleBarToggle().IsOn())
                {
                    BorderToggle().IsOn(true);
                }
                overlappedPresenter.SetBorderAndTitleBar(BorderToggle().IsOn(), TitleBarToggle().IsOn());
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



