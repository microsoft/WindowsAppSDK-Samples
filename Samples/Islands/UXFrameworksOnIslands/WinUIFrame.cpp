// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "WinUIFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"

WinUIFrame::WinUIFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings) :
    LiftedFrame(compositor, settings),
    m_labelVisual(GetOutput(), k_frameName)
{
    m_labelVisual.SetBackgroundColor(ColorUtils::LightYellow());
    InitializeVisualTree(compositor);
}

void WinUIFrame::InitializeVisualTree(const winrt::Compositor& compositor)
{
    // Fill entire bounds with light brown color
    // Eventually do something more interesting (i.e. DrawingIsland or real XamlIsland)
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    auto colorVisualPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_ImageControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(colorVisualPeer->Fragment());
    m_automationTree->AddPeer(colorVisualPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::LightYellow());

    // Insert the label into the tree.
    InsertTextVisual(m_labelVisual, m_automationTree, colorVisualPeer);
}
