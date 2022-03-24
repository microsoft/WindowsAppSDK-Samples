// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_ShortName.g.h"

namespace winrt::CppPackagedAppNotifications2::implementation
{
    struct Scenario2_ShortName : Scenario2_ShortNameT<Scenario2_ShortName>
    {
        Scenario2_ShortName();
    };
}

namespace winrt::CppPackagedAppNotifications2::factory_implementation
{
    struct Scenario2_ShortName : Scenario2_ShortNameT<Scenario2_ShortName, implementation::Scenario2_ShortName>
    {
    };
}
