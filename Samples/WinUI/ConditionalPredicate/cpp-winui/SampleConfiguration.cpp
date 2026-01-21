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

namespace winrt::ConditionalXamlPredicate
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Load Conditional Controls", hstring(name_of<ConditionalXamlPredicate::Scenario1_LoadConditionalControls>())},
            Scenario{ L"Conditional Arguments", hstring(name_of<ConditionalXamlPredicate::Scenario2_ConditionalArguments>())},
        });

    hstring SampleConfig::FeatureName{ L"ConditionalXamlPredicate" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
