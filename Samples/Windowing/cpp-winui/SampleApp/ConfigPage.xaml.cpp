// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ConfigPage.xaml.h"
#if __has_include("ConfigPage.g.cpp")
#include "ConfigPage.g.cpp"
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
    }

    void ConfigPage::ChangeWindowStyle(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        std::string buttonName = to_string(sender.as<Button>().Name());
        OverlappedPresenter customOverlappedPresenter = nullptr;
        
        if (buttonName == "MainWindowBtn")
        {
            customOverlappedPresenter = winrt::OverlappedPresenter::Create();
        }
        else if (buttonName == "ContextMenuBtn")
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

    void ConfigPage::ChangeConfiguration(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_mainAppWindow)
        {
            ToggleSwitch toggleSwitch = sender.as<ToggleSwitch>();
            
            std::string toggleName = to_string(toggleSwitch.Name());
            OverlappedPresenter overlappedPresenter = m_mainAppWindow.Presenter().try_as<OverlappedPresenter>();

            // Change the property that corresponts to the switch that was toggled
            if (toggleName == "FrameToggle")
            {
                // TODO: THIS DOES NOT WORK. FIX IT!
                overlappedPresenter.SetBorderAndTitleBar(FrameToggle().IsOn(), TitleBarToggle().IsOn());
            }
            else if (toggleName == "TitleBarToggle")
            {
                // TODO: THIS DOES NOT WORK. FIX IT!
                overlappedPresenter.SetBorderAndTitleBar(FrameToggle().IsOn(), TitleBarToggle().IsOn());
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
