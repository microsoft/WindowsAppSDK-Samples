// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wil/cppwinrt.h> // Already includes unknwn.h for COM support
#include <wil/resource.h>

#include <winrt/base.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>

#include <winrt/Microsoft.Windows.Widgets.Providers.h>

#include <ppltasks.h>

namespace winrt
{
    namespace Microsoft::Windows::Widgets {};
    using namespace Microsoft::Windows::Widgets;

    namespace Microsoft::Windows::Widgets::Providers {};
    using namespace Microsoft::Windows::Widgets::Providers;
}
