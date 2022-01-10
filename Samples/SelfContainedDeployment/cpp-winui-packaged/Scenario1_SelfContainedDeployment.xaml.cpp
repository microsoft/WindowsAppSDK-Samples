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
    
    void Scenario1_SelfContainedDeployment::SuccessMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Success);
    }

    void Scenario1_SelfContainedDeployment::ErrorMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Something went wrong.", InfoBarSeverity::Error);
    }

    void Scenario1_SelfContainedDeployment::InformationalMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"This is the informational bar.", InfoBarSeverity::Informational);
    }

    void Scenario1_SelfContainedDeployment::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", InfoBarSeverity::Informational);
    }
}
