﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Windows Platform Win32
#include <unknwn.h>
#include <UIAutomationCore.h>
#include <UIAutomationCoreApi.h>
#include <UIAutomationClient.h>

// DWriteWrite and Direct2D for rendering text
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <dwrite_3.h>

// WIL
#include <wil/cppwinrt.h>
#include <wil/Result.h>
#include <wil/Resource.h>

// Windows Platform WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>

// Windows App SDK
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Composition.Interop.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>

//#include <WindowingCoreContentApi.h>
#include <winrt/microsoft.ui.input.inputpretranslatesource.interop.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>

#include <WindowsNumerics.h>

namespace winrt
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::Foundation::Numerics;
    using namespace winrt::Windows::Graphics::DirectX;
    using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
    using namespace winrt::Windows::System;
    using namespace winrt::Windows::UI;

    using namespace winrt::Microsoft::UI;
    using namespace winrt::Microsoft::UI::Composition;
    using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
    using namespace winrt::Microsoft::UI::Content;
    using namespace winrt::Microsoft::UI::Dispatching;
    using namespace winrt::Microsoft::UI::Input;
    using namespace winrt::Microsoft::UI::Windowing;
}

using namespace Windows::Foundation::Numerics;
