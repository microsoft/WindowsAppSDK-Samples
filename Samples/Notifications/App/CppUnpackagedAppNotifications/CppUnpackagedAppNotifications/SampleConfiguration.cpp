// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h"
#include "Notifications\ToastWithAvatar.h"
#include "Notifications\ToastWithTextBox.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Windows::Foundation::Collections;
}

namespace winrt::CppUnpackagedAppNotifications
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ ToastWithAvatar::ScenarioName, hstring(name_of<CppUnpackagedAppNotifications::Scenario1_ToastWithAvatar>())},
            Scenario{ ToastWithTextBox::ScenarioName, hstring(name_of<CppUnpackagedAppNotifications::Scenario2_ToastWithTextBox>())}
        });

    hstring SampleConfig::FeatureName{ L"CppUnpackagedAppNotifications" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
