// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

// C++ Standard headers
#include <string>
#include <vector>
#include <cstdio>
#include <mutex>

// Windows headers
#define NOMINMAX
#include <unknwn.h>
#include <WinUser.h>
#include <windowsx.h> // For GET_X_LPARAM and GET_Y_LPARAM
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <dwrite_3.h>

// WIL headers
#include <wil/cppwinrt.h> // Needs to be included before any C++/WinRT headers
#include <wil/result.h>

// System winrt headers
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Interactions.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.System.h>

// Lifted winrt headers
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.Interactions.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>

// Interop headers
#include <Windows.UI.Composition.Interop.h>
#include <winrt/Microsoft.UI.Dispatching.Interop.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Composition.Interop.h>

// Namespace aliases
#ifdef BUILD_WINDOWS
namespace ABI::Windows::UI::Composition 
{
    using ICompositionDrawingSurfaceInterop = ::Windows::UI::Composition::ICompositionDrawingSurfaceInterop;
    using ICompositionGraphicsDevice = ::Windows::UI::Composition::ICompositionGraphicsDevice;
    using ICompositionGraphicsDeviceInterop = ::Windows::UI::Composition::ICompositionGraphicsDeviceInterop;
    using ICompositorInterop = ::Windows::UI::Composition::ICompositorInterop;
}
#endif

namespace winrt
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::Foundation::Numerics;

    using namespace winrt::Microsoft::UI::Composition;
    using namespace winrt::Microsoft::UI::Composition::Interactions;
    using namespace winrt::Microsoft::UI::Content;
    using namespace winrt::Microsoft::UI::Dispatching;
    using namespace winrt::Microsoft::UI::Input;
    using namespace winrt::Microsoft::UI::Windowing;

    namespace WS = winrt::Windows::System;
    namespace WUC = winrt::Windows::UI::Composition;
    namespace WUCI = winrt::Windows::UI::Composition::Interactions;

    using winrt::Microsoft::UI::GetWindowFromWindowId;
    using winrt::Microsoft::UI::GetWindowIdFromWindow;
    using winrt::Microsoft::UI::WindowId;
}
