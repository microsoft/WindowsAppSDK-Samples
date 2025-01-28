// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h" 

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Windows::Foundation::Collections;
}

namespace winrt::cpp_winui
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Numeric Badge", hstring(name_of<cpp_winui::Scenario1_NumericBadge>())},
            Scenario{ L"Glyph Badge", hstring(name_of<cpp_winui::Scenario2_GlyphBadge>())}
        });

    hstring SampleConfig::FeatureName{ L"cpp_winui" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
