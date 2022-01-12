// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_SelfContainedDeployment.xaml.h"
#if __has_include("Scenario1_SelfContainedDeployment.g.cpp")
#include "Scenario1_SelfContainedDeployment.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::SelfContainedDeployment::implementation
{
    MainPage Scenario1_SelfContainedDeployment::rootPage{ nullptr };

    Scenario1_SelfContainedDeployment::Scenario1_SelfContainedDeployment()
    {
        InitializeComponent();
        Scenario1_SelfContainedDeployment::rootPage = MainPage::Current();
    }
}
