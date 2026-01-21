// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_LoadConditionalControls.xaml.h"
#if __has_include("Scenario1_LoadConditionalControls.g.cpp")
#include "Scenario1_LoadConditionalControls.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::ConditionalXamlPredicate::implementation
{
    MainPage Scenario1_LoadConditionalControls::rootPage{ nullptr };

    Scenario1_LoadConditionalControls::Scenario1_LoadConditionalControls()
    {
        InitializeComponent();
        Scenario1_LoadConditionalControls::rootPage = MainPage::Current();
    }
}
