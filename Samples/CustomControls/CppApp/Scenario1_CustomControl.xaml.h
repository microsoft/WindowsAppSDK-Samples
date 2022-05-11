// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_CustomControl.g.h"

namespace winrt::CppApp::implementation
{
    struct Scenario1_CustomControl : Scenario1_CustomControlT<Scenario1_CustomControl>
    {
        Scenario1_CustomControl();
        //void SuccessMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        //void ErrorMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        //void InformationalMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        //void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static CppApp::MainPage rootPage;
    };
}

namespace winrt::CppApp::factory_implementation
{
    struct Scenario1_CustomControl : Scenario1_CustomControlT<Scenario1_CustomControl, implementation::Scenario1_CustomControl>
    {
    };
}
