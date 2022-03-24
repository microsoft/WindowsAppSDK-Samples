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

namespace winrt::CppPackagedAppNotifications4
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Local Toast with Avatar Image", hstring(name_of<CppPackagedAppNotifications4::Scenario1_ToastWithAvatar>())},
            Scenario{ L"Local Toast with Avatar and Text Box", hstring(name_of<CppPackagedAppNotifications4::Scenario2_ToastWithTextBox>())}
        });

    hstring SampleConfig::FeatureName{ L"CppPackagedAppNotifications4" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
