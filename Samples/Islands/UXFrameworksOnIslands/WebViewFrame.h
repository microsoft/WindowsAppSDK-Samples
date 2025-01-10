// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "SystemFrame.h"
#include "TextVisual.h"

class WebViewFrame final : public SystemFrame
{
public:
    WebViewFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings);

private:
    void InitializeVisualTree(
        const winrt::WUC::Compositor& compositor);

    void HandleContentLayout() override;

    static constexpr wchar_t k_frameName[] = L"WebView";

    void AddContent(_In_z_ WCHAR const* text, IDWriteTextFormat* textFormat);

    static constexpr float k_leftMargin = 10;
    static constexpr float k_topMargin = 10;
    static constexpr float k_rightMargin = 50;
    static constexpr float k_paraGap = 10;
    static constexpr float k_minColumNWidth = 200;

    std::shared_ptr<AutomationPeer> m_colorVisualPeer;
    std::vector<std::unique_ptr<SystemTextVisual>> m_content;
};
