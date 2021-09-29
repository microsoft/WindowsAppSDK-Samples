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
        for (UIElement c : themePanel().Children())
        {
            ElementTheme tag = c.as<RadioButton>().Tag().as<ElementTheme>();
            if (tag == Settings::CurrentTheme)
            {
                RadioButton radioButton = c.as<RadioButton>();
                radioButton.IsChecked(true);
            }
        }
    }

    void SettingsPage::OnThemeRadioButtonChecked(IInspectable const& sender, RoutedEventArgs const&)
    { 
        RadioButton radiobutton = sender.as<RadioButton>();
        ElementTheme selectedTheme = unbox_value<ElementTheme>(radiobutton.Tag());

        Grid content = MainPage::Current().Content().as<Grid>();
        if (content != nullptr)
        {
            content.RequestedTheme(selectedTheme);
            Settings::CurrentTheme = content.RequestedTheme();
        }       
    }
}
