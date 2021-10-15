// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_ShortName.g.h"

namespace winrt::$safeprojectname$::implementation
{
    struct Scenario2_ShortName : Scenario2_ShortNameT<Scenario2_ShortName>
    {
        Scenario2_ShortName();
    };
}

namespace winrt::$safeprojectname$::factory_implementation
{
    struct Scenario2_ShortName : Scenario2_ShortNameT<Scenario2_ShortName, implementation::Scenario2_ShortName>
    {
    };
}
