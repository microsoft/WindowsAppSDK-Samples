// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MyCustomPredicate.g.h"

namespace winrt::ConditionalXamlPredicate::implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate>
    {
        MyCustomPredicate() = default;

        bool Evaluate(hstring const& argument);
    };
}
namespace winrt::ConditionalXamlPredicate::factory_implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate, implementation::MyCustomPredicate>
    {
    };
}