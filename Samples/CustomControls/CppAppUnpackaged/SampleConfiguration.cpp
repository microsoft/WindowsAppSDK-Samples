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

namespace winrt::CppAppUnpackaged
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Scenario 1", hstring(name_of<CppAppUnpackaged::Scenario1_CustomControl>())},
            Scenario{ L"Scenario 2", hstring(name_of<CppAppUnpackaged::Scenario2_UserControl>())}
        });

    hstring SampleConfig::FeatureName{ L"CppAppUnpackaged" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
