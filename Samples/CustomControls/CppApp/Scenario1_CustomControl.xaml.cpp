// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_CustomControl.xaml.h"
#if __has_include("Scenario1_CustomControl.g.cpp")
#include "Scenario1_CustomControl.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::CppApp::implementation
{
    MainPage Scenario1_CustomControl::rootPage{ nullptr };

    Scenario1_CustomControl::Scenario1_CustomControl()
    {
        InitializeComponent();
        Scenario1_CustomControl::rootPage = MainPage::Current();
    }
    
    //void Scenario1_CustomControl::SuccessMessage_Click(IInspectable const&, RoutedEventArgs const&)
    //{
    //    rootPage.NotifyUser(L"Everything was ok!", InfoBarSeverity::Success);
    //}

    //void Scenario1_CustomControl::ErrorMessage_Click(IInspectable const&, RoutedEventArgs const&)
    //{
    //    rootPage.NotifyUser(L"Something went wrong.", InfoBarSeverity::Error);
    //}

    //void Scenario1_CustomControl::InformationalMessage_Click(IInspectable const&, RoutedEventArgs const&)
    //{
    //    rootPage.NotifyUser(L"This is the informational bar.", InfoBarSeverity::Informational);
    //}

    //void Scenario1_CustomControl::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    //{
    //    rootPage.NotifyUser(L"", InfoBarSeverity::Informational);
    //}
}
