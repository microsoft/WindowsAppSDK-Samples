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

namespace winrt::$safeprojectname$
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Scenario 1", hstring(name_of<$safeprojectname$::Scenario1_ShortName>())},
            Scenario{ L"Scenario 2", hstring(name_of<$safeprojectname$::Scenario2_ShortName>())}
        });

    hstring SampleConfig::FeatureName{ L"$safeprojectname$" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
