// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_SelfContainedDeployment.g.h"

namespace winrt::SelfContainedDeployment::implementation
{
    struct Scenario1_SelfContainedDeployment : Scenario1_SelfContainedDeploymentT<Scenario1_SelfContainedDeployment>
    {
        Scenario1_SelfContainedDeployment();
        void SuccessMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ErrorMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void InformationalMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static SelfContainedDeployment::MainPage rootPage;
    };
}

namespace winrt::SelfContainedDeployment::factory_implementation
{
    struct Scenario1_SelfContainedDeployment : Scenario1_SelfContainedDeploymentT<Scenario1_SelfContainedDeployment, implementation::Scenario1_SelfContainedDeployment>
    {
    };
}
