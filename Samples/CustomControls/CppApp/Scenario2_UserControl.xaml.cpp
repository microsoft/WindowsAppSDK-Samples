// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario2_UserControl.xaml.h"
#if __has_include("Scenario2_UserControl.g.cpp")
#include "Scenario2_UserControl.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
}

namespace winrt::CppApp::implementation
{
    Scenario2_UserControl::Scenario2_UserControl()
    {
        InitializeComponent();
    }
}
