// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MyCustomPredicate.h"
#include "MyCustomPredicate.g.cpp"

namespace winrt::ConditionalXamlPredicate::implementation
{
    bool MyCustomPredicate::Evaluate(hstring const& argument)
    {
        if (argument == L"FeatureABCEnabled")
        {
            return true;
        }
        else if (argument == L"FeatureABCDisabled")
        {
            return false;
        }
        else if (argument == L"FeatureXYZEnabled")
        {
            return false;
        }
        else if (argument == L"FeatureXYZDisabled")
        {
            return true;
        }

        throw hresult_invalid_argument();
    }
}
