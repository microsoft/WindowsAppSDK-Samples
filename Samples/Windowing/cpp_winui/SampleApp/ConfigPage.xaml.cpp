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
        AppWindowConfiguration windowConfiguration = nullptr;
        
        if (buttonName == "MainWindowBtn")
        {
            windowConfiguration = winrt::AppWindowConfiguration::CreateDefault();
        }
        else if (buttonName == "ContextMenuBtn")
        {
            windowConfiguration = winrt::AppWindowConfiguration::CreateForContextMenu();
        }
        else if (buttonName == "DialogWindowBtn")
        {
            windowConfiguration = winrt::AppWindowConfiguration::CreateForDialog();
        }
        else if (buttonName == "ToolWindowBtn")
        {
            windowConfiguration = winrt::AppWindowConfiguration::CreateForToolWindow();
        }
        m_mainAppWindow.ApplyConfiguration(windowConfiguration);
    }

    void ConfigPage::ChangeConfiguration(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_mainAppWindow)
        {
            ToggleSwitch toggleSwitch = sender.as<ToggleSwitch>();

            std::string toggleName = to_string(toggleSwitch.Name());
            AppWindowConfiguration windowConfiguration = m_mainAppWindow.Configuration();

            // Change the property that corresponts to the switch that was toggled
            if (toggleName == "FrameToggle")
            {
                windowConfiguration.HasFrame(toggleSwitch.IsOn());
            }
            else if (toggleName == "TitleBarToggle")
            {
                windowConfiguration.HasTitleBar(toggleSwitch.IsOn());
            }
            else if (toggleName == "MaxToggle")
            {
                windowConfiguration.IsMaximizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "MinToggle")
            {
                windowConfiguration.IsMinimizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "AlwaysOnTopToggle")
            {
                windowConfiguration.IsAlwaysOnTop(toggleSwitch.IsOn());
            }
            else if (toggleName == "ResizableToggle")
            {
                windowConfiguration.IsResizable(toggleSwitch.IsOn());
            }
            else if (toggleName == "InUxToggle")
            {
                windowConfiguration.IsShownInSwitchers(toggleSwitch.IsOn());
            }
            m_mainAppWindow.ApplyConfiguration(windowConfiguration);
        }
    }
}
