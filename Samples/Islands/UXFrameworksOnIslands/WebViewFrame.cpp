// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "WebViewFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"

WebViewFrame::WebViewFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor) :
    SystemFrame(compositor, systemCompositor),
    m_labelVisual(GetOutput(), k_frameName, ColorUtils::LightBrown())
{
    InitializeVisualTree(systemCompositor);
}

void WebViewFrame::InitializeVisualTree(
    const winrt::WUC::Compositor& compositor)
{
    // Fill entire bounds with light brown color
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    auto colorVisualPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_ImageControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(colorVisualPeer->Fragment());
    m_automationTree->AddPeer(colorVisualPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::LightBrown());

    // Insert the label into the tree.
    InsertTextVisual(m_labelVisual, m_automationTree, colorVisualPeer);
}
