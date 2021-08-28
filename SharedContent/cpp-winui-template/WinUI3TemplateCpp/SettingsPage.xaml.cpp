#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::WinUI3TemplateCpp::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
    }

    void SettingsPage::OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        /*std::string currentTheme;
        Grid content = MainPage::Current().Content().as<Grid>();
        
        if (content.RequestedTheme() == ElementTheme::Default)
        {
            Application::Current().RequestedTheme() == ApplicationTheme::Dark ? currentTheme = "Dark" : currentTheme = "Light";
        }*/
    }

    void SettingsPage::OnThemeRadioButtonChecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        RadioButton radiobutton = sender.try_as<RadioButton>();
        if (radiobutton != nullptr && radiobutton.Tag() != nullptr)
        {
            winrt::hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());

            Grid content = MainPage::Current().Content().as<Grid>();
           
            //Grid content = radiobutton.XamlRoot().Content().as<Grid>();
            if (winrt::to_string(selectedTheme) == "Dark") {
                content.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Dark);
            }
            else
            {
                content.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Light);
            }
        }
    }
}



