// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MyCustomPredicate.g.h"

namespace winrt::ConditionalXamlPredicate::implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate>
    {
        MyCustomPredicate() = default;

#ifdef WASDK_EXPERIMENTAL
        bool Evaluate(winrt::Windows::Foundation::Collections::IVectorView<hstring> const& arguments);
#endif
    };
}
namespace winrt::ConditionalXamlPredicate::factory_implementation
{
    struct MyCustomPredicate : MyCustomPredicateT<MyCustomPredicate, implementation::MyCustomPredicate>
    {
    };
}