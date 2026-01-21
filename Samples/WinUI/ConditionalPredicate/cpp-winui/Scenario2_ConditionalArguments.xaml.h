// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_ConditionalArguments.g.h"

namespace winrt::ConditionalXamlPredicate::implementation
{
    struct Scenario2_ConditionalArguments : Scenario2_ConditionalArgumentsT<Scenario2_ConditionalArguments>
    {
        Scenario2_ConditionalArguments();
    };
}

namespace winrt::ConditionalXamlPredicate::factory_implementation
{
    struct Scenario2_ConditionalArguments : Scenario2_ConditionalArgumentsT<Scenario2_ConditionalArguments, implementation::Scenario2_ConditionalArguments>
    {
    };
}
