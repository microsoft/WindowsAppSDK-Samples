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
            Settings::CurrentTheme = RequestedTheme() == ElementTheme::Dark ? L"Dark" : L"Light";
        }
        for (UIElement c : themePanel().Children())
        {
            if (c.as<RadioButton>().Tag().as<hstring>() == Settings::CurrentTheme)
            {
                c.as<RadioButton>().IsChecked(true);
            }
        }
    }

    void SettingsPage::OnThemeRadioButtonChecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        RadioButton radiobutton = sender.try_as<RadioButton>();
        if (radiobutton != nullptr && radiobutton.Tag() != nullptr)
        {
            winrt::hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());
            Grid content = radiobutton.XamlRoot().Content().as<Grid>();
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



