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

namespace winrt::SelfContainedDeployment
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Self-Contained Deployment", hstring(name_of<SelfContainedDeployment::Scenario1_SelfContainedDeployment>())}
        });

    hstring SampleConfig::FeatureName{ L"Self-Contained Deployment" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
