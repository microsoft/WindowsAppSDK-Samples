// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_LoadConditionalControls.g.h"

namespace winrt::ConditionalXamlPredicate::implementation
{
    struct Scenario1_LoadConditionalControls : Scenario1_LoadConditionalControlsT<Scenario1_LoadConditionalControls>
    {
        Scenario1_LoadConditionalControls();
    private:
        static ConditionalXamlPredicate::MainPage rootPage;
    };
}

namespace winrt::ConditionalXamlPredicate::factory_implementation
{
    struct Scenario1_LoadConditionalControls : Scenario1_LoadConditionalControlsT<Scenario1_LoadConditionalControls, implementation::Scenario1_LoadConditionalControls>
    {
    };
}
