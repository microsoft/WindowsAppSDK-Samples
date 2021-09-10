//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <SampleConfiguration.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::WinUI3TemplateCpp::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
    }

    void SettingsPage::OnNavigatedTo(NavigationEventArgs const&)
    {
        // Initialize the CurrentTheme on first navigation to the Settings page
        if (Settings::CurrentTheme == L"")
        {
            if (RequestedTheme() == ElementTheme::Dark) 
            {
                Settings::CurrentTheme = L"Dark";
            } 
            else if (RequestedTheme() == ElementTheme::Light)
            {
                Settings::CurrentTheme = L"Light";
            }
            else
            {
                Settings::CurrentTheme = L"Default";
            }   
        }

        for (UIElement c : themePanel().Children())
        {
            hstring tag = c.as<RadioButton>().Tag().as<hstring>();
            if (tag == Settings::CurrentTheme)
            {
                RadioButton radioButton = c.as<RadioButton>();
                radioButton.IsChecked(IReference<bool>{true});
            }
        }
    }

    void SettingsPage::OnThemeRadioButtonChecked(IInspectable const& sender, RoutedEventArgs const&)
    { 
        RadioButton radiobutton = sender.as<RadioButton>();
        hstring selectedTheme = unbox_value<winrt::hstring>(radiobutton.Tag());

        if (selectedTheme != L"") 
        {
            Grid content = MainPage().Current().Content().as<Grid>();
            if (selectedTheme == L"Dark")
            {
                content.RequestedTheme(ElementTheme::Dark);
                Settings::CurrentTheme = L"Dark";
            }
            else if (selectedTheme == L"Light")
            {
                content.RequestedTheme(ElementTheme::Light);
                Settings::CurrentTheme = L"Light";
            }
            else
            {
                content.RequestedTheme(ElementTheme::Default);
                Settings::CurrentTheme = L"Default";
            }
        }      
    }
}