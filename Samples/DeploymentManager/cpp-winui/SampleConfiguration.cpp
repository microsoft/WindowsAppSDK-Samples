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

namespace winrt::DeploymentManagerSample
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Get Status", hstring(name_of<DeploymentManagerSample::Scenario1_GetStatus>())},
            Scenario{ L"Initialize", hstring(name_of<DeploymentManagerSample::Scenario2_Initialize>())}
        });

    hstring SampleConfig::FeatureName{ L"DeploymentManager" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
