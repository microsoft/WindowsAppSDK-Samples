// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_UserControl.g.h"

namespace winrt::CppAppUnpackaged::implementation
{
    struct Scenario2_UserControl : Scenario2_UserControlT<Scenario2_UserControl>
    {
        Scenario2_UserControl();
    };
}

namespace winrt::CppAppUnpackaged::factory_implementation
{
    struct Scenario2_UserControl : Scenario2_UserControlT<Scenario2_UserControl, implementation::Scenario2_UserControl>
    {
    };
}
