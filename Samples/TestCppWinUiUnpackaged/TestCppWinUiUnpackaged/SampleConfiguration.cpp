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

namespace winrt::TestCppWinUiUnpackaged
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Local Toast with Avatar Image", hstring(name_of<TestCppWinUiUnpackaged::Scenario1_ToastWithAvatar>())},
        });

    hstring SampleConfig::FeatureName{ L"TestCppWinUiUnpackaged" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
