#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <SampleConfiguration.h>

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
        Grid content = MainPage::Current().Content().as<Grid>();
        if (winrt::WinUI3TemplateCpp::Settings::CurrentTheme == L"")
        {
            //auto t = winrt::WinUI3TemplateCpp::Application::RequestedTheme();
            //auto t = RequestedTheme(); // t is ElementTheme::Default ...
            
            
            /*if (t == ElementTheme::Light) {

            }*/
            if (RequestedTheme() == ElementTheme::Dark) 
            {
                Settings::CurrentTheme = L"Dark";
            } else if (RequestedTheme() == ElementTheme::Light)
            {
                Settings::CurrentTheme = L"Light";
            }
            else
            {
                Settings::CurrentTheme = L"Default";
            }
           
            //Settings::CurrentTheme = RequestedTheme() == ElementTheme::Dark ? L"Dark" : L"Light";
        }
        for (UIElement c : themePanel().Children())
        {
            hstring tag = c.as<RadioButton>().Tag().as<hstring>();
            if (tag == Settings::CurrentTheme)
            {
                RadioButton radioButton = c.as<RadioButton>();
                radioButton.IsChecked(Windows::Foundation::IReference<bool>{true});
            }
        }
    }

    void SettingsPage::OnThemeRadioButtonChecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        RadioButton radiobutton = sender.as<RadioButton>();
        if (radiobutton != nullptr && radiobutton.Tag() != nullptr)
        {
            winrt::hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());
            Grid content = MainPage().Current().Content().as<Grid>();
            if (selectedTheme == L"Dark") 
            {
               
                //radiobutton.XamlRoot().Content().as<Grid>().RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Dark);
                content.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Dark);
                Settings::CurrentTheme = L"Dark";
            }
            else if (selectedTheme == L"Light")
            {
                //radiobutton.XamlRoot().Content().as<Grid>().RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Light);
                content.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Light);
                Settings::CurrentTheme = L"Light";
            }
            else
            {
                content.RequestedTheme(Microsoft::UI::Xaml::ElementTheme::Default);
                Settings::CurrentTheme = L"Default";
            }
        }
    }
}



