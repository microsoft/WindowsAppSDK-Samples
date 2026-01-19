// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MyCustomPredicate.g.h"

namespace winrt::ConditionalXamlPredicate::implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate>
    {
        MyCustomPredicate() = default;

        bool Evaluate(winrt::Windows::Foundation::Collections::IVectorView<hstring> const& arguments);
    };
}
namespace winrt::ConditionalXamlPredicate::factory_implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate, implementation::MyCustomPredicate>
    {
    };
}