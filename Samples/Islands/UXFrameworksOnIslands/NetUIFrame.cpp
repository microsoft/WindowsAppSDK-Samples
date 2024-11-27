// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "NetUIFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"

NetUIFrame::NetUIFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor) :
    SystemFrame(compositor, systemCompositor),
    m_labelVisual(GetOutput(), k_frameName, ColorUtils::LightBlue())
{
    InitializeVisualTree(systemCompositor);
}

void NetUIFrame::ConnectFrame(
    IFrame* frame)
{
    auto childContentVisualNode = m_childContentPeer->VisualNode();
    m_childContentLink = ConnectChildFrame(frame, childContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_childContentPeer->SetChildContentLink(m_childContentLink);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_childContentPeer->SetChildFrame(frame);
    }

    HandleContentLayout();
}

void NetUIFrame::InitializeVisualTree(
    const winrt::WUC::Compositor& compositor)
{
    // Fill our bounds with a light blue color
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    auto colorVisualPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_PaneControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(colorVisualPeer->Fragment());
    m_automationTree->AddPeer(colorVisualPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::LightBlue());

    // Insert the label into the tree.
    InsertTextVisual(m_labelVisual, m_automationTree, colorVisualPeer);

    // Create a visual that is inset by 10 pixels on all sides
    // This will hold any child frames
    auto childContentVisual = compositor.CreateContainerVisual();
    auto childContentVisualNode = VisualTreeNode::Create(childContentVisual.as<::IUnknown>());
    m_childContentPeer = m_automationTree->CreatePeer(childContentVisualNode, L"NetUI Content", UIA_PaneControlTypeId);
    colorVisualNode->AddChild(childContentVisualNode);
    colorVisualPeer->Fragment()->AddChildToEnd(m_childContentPeer->Fragment());
    m_automationTree->AddPeer(m_childContentPeer);
    float labelHeight = m_labelVisual.Measure().Height;
    VisualUtils::LayoutAsInset(childContentVisual, k_inset, labelHeight, k_inset, k_inset);
}

void NetUIFrame::ActivateInteractionTracker(
    const winrt::WUC::Compositor& compositor,
    const winrt::WUC::Visual& parentVisual,
    const winrt::WUC::Visual& childVisual)
{
    // Setup the interaction tracker
    m_interactionTracker = winrt::WUCI::InteractionTracker::Create(compositor);
    m_source = winrt::WUCI::VisualInteractionSource::Create(parentVisual);

    m_interactionTracker.InteractionSources().Add(m_source);

    // Set initial values on interaction tracker source
    m_source.ManipulationRedirectionMode(winrt::WUCI::VisualInteractionSourceRedirectionMode::CapableTouchpadOnly);
    m_source.PositionXSourceMode(winrt::WUCI::InteractionSourceMode::EnabledWithInertia);
    m_source.PositionYSourceMode(winrt::WUCI::InteractionSourceMode::EnabledWithInertia);
    m_source.ScaleSourceMode(winrt::WUCI::InteractionSourceMode::EnabledWithInertia);
    m_source.IsPositionXRailsEnabled(false);
    m_source.IsPositionYRailsEnabled(false);

    m_interactionTracker.MaxScale(10);
    m_interactionTracker.MinScale(0.1f);
    m_interactionTracker.MaxPosition({k_size / 2, k_size / 2, 0});
    m_interactionTracker.MinPosition({30 - (k_size / 2), 30 - (k_size / 2), 0});

    // Setup expression animations for scaling
    auto scaleExpression = compositor.CreateExpressionAnimation(L"Vector3(tracker.Scale, tracker.Scale, 1.0)");
    scaleExpression.SetReferenceParameter(L"tracker", m_interactionTracker);
    childVisual.StartAnimation(L"Scale", scaleExpression);

    // Setup expression animation for offset
    m_offsetAnimation = compositor.CreateExpressionAnimation(L"initialOffset - tracker.Position");
    m_offsetAnimation.SetReferenceParameter(L"tracker", m_interactionTracker);
    m_offsetAnimation.SetVector3Parameter(L"initialOffset", {k_inset, k_inset, 0});
    childVisual.StartAnimation(L"Offset", m_offsetAnimation);

    // Setup input
    m_pointerSource = winrt::InputPointerSource::GetForIsland(GetIsland());
    m_pointerSource.PointerPressed([this](auto& /*sender*/, auto& args) {
        auto point = args.CurrentPoint();
        this->ActivateForPointer(point);
    });
}

void NetUIFrame::ActivateForPointer(
    const winrt::Microsoft::UI::Input::PointerPoint& point)
{
    if (m_source != nullptr)
    {
        m_source.TryRedirectForManipulation(point.as<winrt::Windows::UI::Input::PointerPoint>());
    }
}

void NetUIFrame::HandleContentLayout()
{
    SystemFrame::HandleContentLayout();

    if (nullptr != m_childContentLink)
    {
        auto childContentVisualNode = m_childContentPeer->VisualNode();
        m_childContentLink.TransformMatrix(childContentVisualNode->Transform3x2());

        m_childContentLink.ActualSize(childContentVisualNode->Size());
    }
}
