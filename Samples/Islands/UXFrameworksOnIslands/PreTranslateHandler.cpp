// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "LiftedFrame.h"
#include "PreTranslateHandler.h"

PreTranslateHandler::PreTranslateHandler(_In_ LiftedFrame* liftedFrame)
{
    m_liftedFrameWeak = liftedFrame;
}

PreTranslateHandler::~PreTranslateHandler()
{
    m_liftedFrameWeak = nullptr;
}

IFACEMETHODIMP
PreTranslateHandler::OnDirectMessage(
    winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop* /*source*/,
    const MSG* msg,
    UINT keyboardModifiers,
    _Inout_ bool* handled)
{
    *handled = false;

    if (m_liftedFrameWeak != nullptr)
    {
        m_liftedFrameWeak->OnPreTranslateDirectMessage(msg, keyboardModifiers, handled);
    }

    return S_OK;
}

IFACEMETHODIMP
PreTranslateHandler::OnTreeMessage(
    winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop* /*source*/,
    const MSG* msg,
    UINT keyboardModifiers,
    _Inout_ bool* handled)
{
    *handled = false;

    if (m_liftedFrameWeak != nullptr)
    {
        m_liftedFrameWeak->OnPreTranslateTreeMessage(msg, keyboardModifiers, handled);
    }

    return S_OK;
}