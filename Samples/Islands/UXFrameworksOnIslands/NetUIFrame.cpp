// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "NetUIFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"
#include "FrameDocker.h"
#include "FocusManager.h"

NetUIFrame::NetUIFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings) :
    SystemFrame(queue, systemCompositor, settings),
    m_labelVisual(GetOutput(), k_frameName),
    m_acceleratorVisual(GetOutput(), L"2")
{
    m_labelVisual.SetBackgroundColor(ColorUtils::LightBlue());
    InitializeVisualTree(systemCompositor);
}

bool NetUIFrame::SystemPreTranslateMessage(
    UINT message,
    WPARAM wParam,
    LPARAM /*lParam*/)
{
    if (message == WM_SYSKEYDOWN || message == WM_KEYDOWN)
    {
        if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Menu)
        {
            m_acceleratorActive = !m_acceleratorActive;
            m_acceleratorVisual.GetVisual().IsVisible(m_acceleratorActive);
        }
        else {
            m_acceleratorActive = false;
            m_acceleratorVisual.GetVisual().IsVisible(false);
            if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Number2)
            {
                // Take focus
                GetFocusList()->GetManager()->SetFocusToVisual(GetRootVisualTreeNode(), GetRootVisualTreeNode()->OwningFocusList());
            }
        }
    }
    return false;
}

void NetUIFrame::ConnectFrame(
    IFrame* frame)
{
    auto childContentVisualNode = m_childContentPeer->VisualNode();
    m_childSiteLink = ConnectChildFrame(frame, childContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_childContentPeer->SetChildSiteLink(m_childSiteLink);

        // Focus flows through the InputFocusNavigationHost API when the child is lifted content.
        m_focusList->SetChildSiteLink(m_childSiteLink, 1);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_childContentPeer->SetChildFrame(frame);

        // Set up hit testing so these visuals are treated as parent / child in the hit test tree.
        // This is needed because a child system island does not implicitly get pointer events and
        // needs to have hit testing forwarded by the parent island.
        HitTestContext::ConfigureCrossTreeConnection(childContentVisualNode, frame->GetRootVisualTreeNode());

        // Focus lists are directly linked together for a child system frame.
        m_focusList->SetChildFocusList(frame->GetFocusList(), 1);
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
    m_acceleratorVisual.SetBackgroundColor(winrt::Microsoft::UI::Colors::OldLace());
    m_acceleratorVisual.GetVisual().Offset(winrt::Windows::Foundation::Numerics::float3(m_labelVisual.Size().Width+3, 0, 0));
    m_acceleratorVisual.GetVisual().IsVisible(false);
    InsertTextVisual(m_acceleratorVisual, m_automationTree, colorVisualPeer);

    // Create a visual that is inset by 10 pixels on all sides
    // This will hold any child frames
    m_childContentVisual = compositor.CreateContainerVisual();
    auto childContentVisualNode = VisualTreeNode::Create(m_childContentVisual.as<::IUnknown>());
    m_childContentPeer = m_automationTree->CreatePeer(childContentVisualNode, L"NetUI Content", UIA_PaneControlTypeId);
    colorVisualNode->AddChild(childContentVisualNode);
    colorVisualPeer->Fragment()->AddChildToEnd(m_childContentPeer->Fragment());
    m_automationTree->AddPeer(m_childContentPeer);

    m_focusList->AddPlaceholder(); // Child content - Index 1 (root visual is always 0)
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
    // Create a helper object for setting visual positions.
    // The docker snaps to pixels unless we pass zero as the rasterization scale.
    float rasterizationScale = GetIsland().RasterizationScale();
    float dockerRasterizationScale = !GetSetting(Setting_DisablePixelSnapping) ? rasterizationScale : 0.0f;
    FrameDocker docker(GetIsland().ActualSize(), dockerRasterizationScale);

    docker.InsetTop(m_labelVisual.Size().Height);
    docker.InsetLeft(k_inset);
    docker.InsetRight(k_inset);
    docker.InsetBottom(k_inset);

    docker.DockFill(m_childContentVisual);
    
    SystemFrame::HandleContentLayout();

    if (nullptr != m_childSiteLink)
    {
        auto childContentVisualNode = m_childContentPeer->VisualNode();
        m_childSiteLink.LocalToParentTransformMatrix(childContentVisualNode->Transform4x4());

        m_childSiteLink.ActualSize(childContentVisualNode->Size());
    }
}
