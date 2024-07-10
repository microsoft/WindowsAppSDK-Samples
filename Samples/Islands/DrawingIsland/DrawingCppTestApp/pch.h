﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>

#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include "winrt/DrawingIslandComponents.h"

namespace winrt
{
    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Content;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Input;
    using namespace Microsoft::UI::Windowing;

    using namespace DrawingIslandComponents;
}
