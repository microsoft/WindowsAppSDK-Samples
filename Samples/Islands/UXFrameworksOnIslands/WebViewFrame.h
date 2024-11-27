// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "SystemFrame.h"
#include "TextVisual.h"

class WebViewFrame final : public SystemFrame
{
public:
    WebViewFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor);

private:
    void InitializeVisualTree(
        const winrt::WUC::Compositor& compositor);

    static constexpr wchar_t k_frameName[] = L"WebView";
    SystemTextVisual m_labelVisual;
    float m_labelDpiScale = 0.0f;
};
