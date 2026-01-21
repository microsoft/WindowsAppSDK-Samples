// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MyCustomPredicate.h"
#include "MyCustomPredicate.g.cpp"

namespace winrt::ConditionalXamlPredicate::implementation
{
    bool MyCustomPredicate::Evaluate(winrt::Windows::Foundation::Collections::IVectorView<hstring> const& arguments)
    {
        auto argsCount = arguments.Size();
        if (argsCount >= 2)
        {
            hstring firstArg = arguments.GetAt(0);
			hstring secondArg = arguments.GetAt(1);
            if (firstArg == L"FeatureABC")
            {
				// For demo purpose, return true if second argument is "true", false otherwise
                return secondArg == L"true";
            }
            else if (firstArg == L"FeatureXYZ")
            {
				// Marking FeatureXYZ as disabled
                return secondArg == L"false";
			}
        }
		throw hresult_not_implemented();
    }
}
