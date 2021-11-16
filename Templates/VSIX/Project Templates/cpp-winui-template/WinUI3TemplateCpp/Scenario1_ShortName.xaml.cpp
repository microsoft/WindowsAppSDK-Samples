// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_ShortName.xaml.h"
#if __has_include("Scenario1_ShortName.g.cpp")
#include "Scenario1_ShortName.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::$safeprojectname$::implementation
{
    MainPage Scenario1_ShortName::rootPage{ nullptr };

    Scenario1_ShortName::Scenario1_ShortName()
    {
        InitializeComponent();
        Scenario1_ShortName::rootPage = MainPage::Current();
    }
    
    void Scenario1_ShortName::SuccessMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Success);
    }

    void Scenario1_ShortName::ErrorMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Something went wrong.", InfoBarSeverity::Error);
    }

    void Scenario1_ShortName::InformationalMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"This is the informational bar.", InfoBarSeverity::Informational);
    }

    void Scenario1_ShortName::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", InfoBarSeverity::Informational);
    }
}
