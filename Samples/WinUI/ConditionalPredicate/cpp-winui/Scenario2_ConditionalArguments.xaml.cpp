// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario2_ConditionalArguments.xaml.h"
#if __has_include("Scenario2_ConditionalArguments.g.cpp")
#include "Scenario2_ConditionalArguments.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ConditionalXamlPredicate::implementation
{
    Scenario2_ConditionalArguments::Scenario2_ConditionalArguments()
    {
        InitializeComponent();
    }
}
