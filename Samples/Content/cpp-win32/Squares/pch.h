// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <unknwn.h>
#include <wil/cppwinrt.h>
#include <wil/Result.h>
#include <wil/Resource.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include "winrt/Windows.Foundation.Collections.h"
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>

// UIA automation related headers
#include <uiautomationcore.h>
#include <uiautomationcoreapi.h>
#include <UIAutomationClient.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::System;
    using namespace Windows::UI;

    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Content;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Input;
    using namespace Microsoft::UI::Windowing;
}

