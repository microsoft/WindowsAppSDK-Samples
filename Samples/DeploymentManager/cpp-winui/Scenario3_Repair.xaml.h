// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario3_Repair.g.h"
#include "pch.h"

namespace winrt::DeploymentManagerSample::implementation
{
    struct Scenario3_Repair : Scenario3_RepairT<Scenario3_Repair>
    {
        Scenario3_Repair();
        void UpdateRepairMessages();
        void UpdateDeploymentResultMessages(winrt::Microsoft::Windows::ApplicationModel::WindowsAppRuntime::DeploymentResult deploymentResult);
        void RepairScenarioButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ForceRepairScenarioButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::DeploymentManagerSample::factory_implementation
{
    struct Scenario3_Repair : Scenario3_RepairT<Scenario3_Repair, implementation::Scenario3_Repair>
    {
    };
}
