// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_GetStatus.g.h"

namespace winrt::DeploymentManagerSample::implementation
{
    struct Scenario1_GetStatus : Scenario1_GetStatusT<Scenario1_GetStatus>
    {
        Scenario1_GetStatus();
        void GetStatusButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::DeploymentManagerSample::factory_implementation
{
    struct Scenario1_GetStatus : Scenario1_GetStatusT<Scenario1_GetStatus, implementation::Scenario1_GetStatus>
    {
    };
}
