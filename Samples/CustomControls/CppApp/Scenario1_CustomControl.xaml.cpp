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
}
