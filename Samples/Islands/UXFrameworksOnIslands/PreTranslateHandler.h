// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <winrt/Microsoft.UI.Input.InputPreTranslateSource.Interop.h>

class LiftedFrame;

struct PreTranslateHandler : winrt::implements<PreTranslateHandler, winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceHandler>
{
public:
    PreTranslateHandler(LiftedFrame* liftedFrame);
    ~PreTranslateHandler();

    // IInputKeyboardSourcePreTranslateHandler methods
    IFACEMETHOD(OnDirectMessage)(
        winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop* source,
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled);

    IFACEMETHOD(OnTreeMessage)(
        winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop* source,
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled);

private:
    LiftedFrame* m_liftedFrameWeak{};
};