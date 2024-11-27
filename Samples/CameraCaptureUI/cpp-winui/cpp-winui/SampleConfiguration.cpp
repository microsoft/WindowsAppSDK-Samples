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
            Scenario{ L"Capture Photo", hstring(name_of<cpp_winui::Scenario1_CapturePhoto>())},
            Scenario{ L"Capture Video", hstring(name_of<cpp_winui::Scenario2_CaptureVideo>())}
        });

    hstring SampleConfig::FeatureName{ L"cpp_winui" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
