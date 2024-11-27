// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "LiftedFrame.h"
#include "TextVisual.h"

class WinUIFrame final : public LiftedFrame
{
public:
    explicit WinUIFrame(const winrt::Compositor& compositor);

private:
    void InitializeVisualTree(const winrt::Compositor& compositor);

    static constexpr wchar_t k_frameName[] = L"WinUI";
    LiftedTextVisual m_labelVisual;
    float m_labelDpiScale = 0.0f;
};
