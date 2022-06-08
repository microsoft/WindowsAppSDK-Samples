// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <SampleConfiguration.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Navigation;
    using namespace Windows::Foundation;
}

namespace winrt::SelfContainedDeployment::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
    }

    void SettingsPage::OnNavigatedTo(NavigationEventArgs const&)
    {
        for (UIElement&& c : themePanel().Children())
        {
            auto tag = c.as<RadioButton>().Tag().as<ElementTheme>();
            if (tag == SampleConfig::CurrentTheme)
            {
                auto radioButton = c.as<RadioButton>();
                radioButton.IsChecked(true);
            }
        }
    }

    void SettingsPage::OnThemeRadioButtonChecked(IInspectable const& sender, RoutedEventArgs const&)
    { 
        auto radiobutton = sender.as<RadioButton>();
        auto selectedTheme = unbox_value<ElementTheme>(radiobutton.Tag());

        auto content = MainPage::Current().Content().as<Grid>();
        if (content != nullptr)
        {
            content.RequestedTheme(selectedTheme);
            SampleConfig::CurrentTheme = content.RequestedTheme();
        }  
    }
}
