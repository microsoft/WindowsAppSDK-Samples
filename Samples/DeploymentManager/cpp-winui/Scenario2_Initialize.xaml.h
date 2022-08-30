// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_Initialize.g.h"
#include "pch.h"

namespace winrt::DeploymentManagerSample::implementation
{
    struct Scenario2_Initialize : Scenario2_InitializeT<Scenario2_Initialize>
    {
        Scenario2_Initialize();
        void UpdateInitializeMessages();
        void UpdateDeploymentResultMessages(winrt::Microsoft::Windows::ApplicationModel::WindowsAppRuntime::DeploymentResult deploymentResult);
        void InitializeScenarioButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ForceInitializeScenarioButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::DeploymentManagerSample::factory_implementation
{
    struct Scenario2_Initialize : Scenario2_InitializeT<Scenario2_Initialize, implementation::Scenario2_Initialize>
    {
    };
}
