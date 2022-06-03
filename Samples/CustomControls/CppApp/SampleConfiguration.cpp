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

namespace winrt::CppApp
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Custom Control", hstring(name_of<CppApp::Scenario1_CustomControl>())},
            Scenario{ L"User Control", hstring(name_of<CppApp::Scenario2_UserControl>())}
        });

    hstring SampleConfig::FeatureName{ L"CppApp" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
