// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"

namespace winrt::CppApp
{
    struct SampleConfig
    {
    public:
        static hstring FeatureName;
        static Microsoft::UI::Xaml::ElementTheme CurrentTheme;
    };
}
