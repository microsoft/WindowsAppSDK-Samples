// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_CustomControl.g.h"

namespace winrt::CppAppUnpackaged::implementation
{
    struct Scenario1_CustomControl : Scenario1_CustomControlT<Scenario1_CustomControl>
    {
        Scenario1_CustomControl();
    private:
        static CppAppUnpackaged::MainPage rootPage;
    };
}

namespace winrt::CppAppUnpackaged::factory_implementation
{
    struct Scenario1_CustomControl : Scenario1_CustomControlT<Scenario1_CustomControl, implementation::Scenario1_CustomControl>
    {
    };
}
