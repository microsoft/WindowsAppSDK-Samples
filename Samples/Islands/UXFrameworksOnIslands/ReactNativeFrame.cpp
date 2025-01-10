// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "ReactNativeFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"
#include "FrameDocker.h"

ReactNativeFrame::ReactNativeFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings) :
    LiftedFrame(compositor, settings),
    m_labelVisual(GetOutput(), k_frameName)
{
    InitializeVisualTree(compositor);
    RegisterForInputEvents();
}

void ReactNativeFrame::ConnectLeftFrame(
    IFrame* frame)
{
    m_leftFrame = frame;
    auto leftContentVisualNode = m_leftContentPeer->VisualNode();
    m_leftChildSiteLink = ConnectChildFrame(frame, leftContentVisualNode->Visual().as<winrt::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_leftContentPeer->SetChildSiteLink(m_leftChildSiteLink);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_leftContentPeer->SetChildFrame(frame);
    }

    HandleContentLayout();
}

void ReactNativeFrame::ConnectRightFrame(
    IFrame* frame)
{
    m_rightFrame = frame;
    auto rightContentVisualNode = m_rightContentPeer->VisualNode();
    m_rightChildSiteLink = ConnectChildFrame(frame, rightContentVisualNode->Visual().as<winrt::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_rightContentPeer->SetChildSiteLink(m_rightChildSiteLink);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_rightContentPeer->SetChildFrame(frame);
    }

    HandleContentLayout();
}

winrt::com_ptr<::IRawElementProviderFragmentRoot> ReactNativeFrame::GetFragmentRootProviderForChildFrame(_In_ IFrame const* const) const
{
    return m_automationTree->Root();
}

winrt::com_ptr<::IRawElementProviderFragment> ReactNativeFrame::GetNextSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // We've set up our automation tree in a way so that the frame providers are solitary children of their parent.
    return nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> ReactNativeFrame::GetParentProviderForChildFrame(_In_ IFrame const* const sender) const
{
    if (sender == m_leftFrame)
    {
        return m_leftContentPeer->Fragment();
    }
    else if (sender == m_rightFrame)
    {
        return m_rightContentPeer->Fragment();
    }

    return nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> ReactNativeFrame::GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // We've set up our automation tree in a way so that the frame providers are solitary children of their parent.
    return nullptr;
}

void ReactNativeFrame::InitializeVisualTree(
    const winrt::Compositor& compositor)
{
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    m_backgroundPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_PaneControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(m_backgroundPeer->Fragment());
    m_automationTree->AddPeer(m_backgroundPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::LightGreen());

    // Insert the label into the tree.
    InsertTextVisual(m_labelVisual, m_automationTree, m_backgroundPeer);

    // Partition the layout into 3 regions: top, bottom left, bottom right.
    m_topContentVisual = compositor.CreateContainerVisual();
    auto topContentVisualTreeNode = VisualTreeNode::Create(m_topContentVisual.as<::IUnknown>());
    m_topContentPeer = m_automationTree->CreatePeer(topContentVisualTreeNode, L"ReactNative Top", UIA_PaneControlTypeId);
    colorVisualNode->AddChild(topContentVisualTreeNode);
    m_backgroundPeer->Fragment()->AddChildToEnd(m_topContentPeer->Fragment());
    m_automationTree->AddPeer(m_topContentPeer);

    m_leftRootVisual = compositor.CreateContainerVisual();
    auto leftRootNode = VisualTreeNode::Create(m_leftRootVisual.as<::IUnknown>());
    m_leftContentPeer = m_automationTree->CreatePeer(leftRootNode, L"ReactNative Left", UIA_PaneControlTypeId);
    colorVisualNode->AddChild(leftRootNode);
    m_backgroundPeer->Fragment()->AddChildToEnd(m_leftContentPeer->Fragment());
    m_automationTree->AddPeer(m_leftContentPeer);

    m_rightRootVisual = compositor.CreateContainerVisual();
    auto rightRootNode = VisualTreeNode::Create(m_rightRootVisual.as<::IUnknown>());
    m_rightContentPeer = m_automationTree->CreatePeer(rightRootNode, L"ReactNative Right", UIA_PaneControlTypeId);
    colorVisualNode->AddChild(rightRootNode);
    m_backgroundPeer->Fragment()->AddChildToEnd(m_rightContentPeer->Fragment());

    // This is just a dummy structural visual to hold the click squares
    auto clickSquareRoot = compositor.CreateContainerVisual();
    m_clickSquareRoot = VisualTreeNode::Create(clickSquareRoot.as<::IUnknown>());
    colorVisualNode->AddChild(m_clickSquareRoot);

    InitializeReactNativeContent();
}

void ReactNativeFrame::InitializeReactNativeContent()
{
    // But for now just fill with solid color
    auto topContentRootVisualNode = m_topContentPeer->VisualNode();
    auto topContentRoot = topContentRootVisualNode->Visual().as<winrt::ContainerVisual>();
    auto compositor = topContentRoot.Compositor();

    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    m_topColorPeer = m_automationTree->CreatePeer(colorVisualNode, L"ReactNative Top Content", UIA_ImageControlTypeId);
    topContentRootVisualNode->AddChild(colorVisualNode);
    m_topContentPeer->Fragment()->AddChildToEnd(m_topColorPeer->Fragment());
    m_automationTree->AddPeer(m_topColorPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::DarkGreen());
}

void ReactNativeFrame::ActivateInteractionTracker(
    const winrt::Compositor& compositor,
    const winrt::Visual& parentVisual,
    const winrt::Visual& childVisual)
{
    // Set up interaction tracker
    m_interactionTracker = winrt::InteractionTracker::Create(compositor);
    m_source = winrt::VisualInteractionSource::Create(parentVisual);

    m_interactionTracker.InteractionSources().Add(m_source);

    // Set initial values on interaction tracker source
    m_source.ManipulationRedirectionMode(winrt::VisualInteractionSourceRedirectionMode::CapableTouchpadOnly);
    m_source.PositionXSourceMode(winrt::InteractionSourceMode::EnabledWithInertia);
    m_source.PositionYSourceMode(winrt::InteractionSourceMode::EnabledWithInertia);
    m_source.ScaleSourceMode(winrt::InteractionSourceMode::EnabledWithInertia);
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
}

void ReactNativeFrame::RegisterForInputEvents()
{
    auto island = GetIsland();
    m_pointerSource = winrt::InputPointerSource::GetForIsland(island);
    m_keyboardSource = winrt::InputKeyboardSource::GetForIsland(island);
    m_activationListener = winrt::InputActivationListener::GetForIsland(island);

    m_pointerSource.PointerPressed(
        [this](auto&& /*sender*/, auto&& args)
        {
            auto point = args.CurrentPoint();
            if (point.PointerDeviceType() != winrt::PointerDeviceType::Mouse)
            {
                ActivateForPointer(point);
            }
            else
            {
                OnClick(point);
            }
        });

    m_keyboardSource.KeyDown(
        [this](auto&& /*sender*/, auto&& args)
        {
            OnKeyPress(args.VirtualKey());
        });

    m_activationListener.InputActivationChanged(
        [this](auto&& /*sender*/, auto&& /*args*/)
        {
            bool isActive = m_activationListener.State() == winrt::InputActivationState::Activated;
            OnActivationChanged(isActive);
        });
}

void ReactNativeFrame::OnClick(
    const winrt::PointerPoint& point)
{
    // Unconditionally take focus
    auto focusController = winrt::InputFocusController::GetForIsland(GetIsland());
    focusController.TrySetFocus();

    auto clickSquareRootVisual = m_clickSquareRoot->Visual().as<winrt::ContainerVisual>();
    auto compositor = clickSquareRootVisual.Compositor();

    // Create our 10x10 blue square
    auto blueVisual = compositor.CreateSpriteVisual();
    auto blueVisualNode = VisualTreeNode::Create(blueVisual.as<::IUnknown>());
    m_clickSquareRoot->AddChild(blueVisualNode);

    blueVisual.Size({10.0f, 10.0f});

    // Color it blue
    auto blue = winrt::Windows::UI::Colors::DarkBlue();
    blueVisual.Brush(compositor.CreateColorBrush(blue));
    blueVisual.Offset({point.Position().X - 5.0f, point.Position().Y - 5.0f, 0.0});
}

void ReactNativeFrame::ActivateForPointer(
    const winrt::PointerPoint& point)
{
    if (m_source != nullptr)
    {
        m_source.TryRedirectForManipulation(point);
    }
}

void ReactNativeFrame::OnKeyPress(
    winrt::Windows::System::VirtualKey key)
{
    if (key == winrt::Windows::System::VirtualKey::Escape)
    {
        m_clickSquareRoot->RemoveAllChildren();
    }
    else if (key == winrt::Windows::System::VirtualKey::A ||
             key == winrt::Windows::System::VirtualKey::B ||
             key == winrt::Windows::System::VirtualKey::C)
    {
        auto compositor = GetRootVisualTreeNode()->Visual().as<winrt::Visual>().Compositor();
        winrt::Windows::UI::Color color =
            (key == winrt::Windows::System::VirtualKey::A) ? ColorUtils::DarkGreen() :
            (key == winrt::Windows::System::VirtualKey::B) ? winrt::Windows::UI::Colors::Purple() :
            winrt::Windows::UI::Colors::Lavender();

        auto brush = compositor.CreateColorBrush(color);
        m_topColorPeer->VisualNode()->Visual().as<winrt::SpriteVisual>().Brush(brush);
    }
}

void ReactNativeFrame::OnActivationChanged(
    bool isActive)
{
    // If inactive, use a more muted color
    auto color = isActive ? ColorUtils::LightGreen() : ColorUtils::FromHexValue(0xff9cc9af);

    auto colorVisual = m_backgroundPeer->VisualNode()->Visual().as<winrt::SpriteVisual>();
    auto colorBrush = colorVisual.Compositor().CreateColorBrush(color);
    colorVisual.Brush(colorBrush);
}

void ReactNativeFrame::HandleContentLayout()
{
    // Visual layout:
    //
    //  +-------------------------------------------------+
    //  | Label                                           |
    //  +  +-------------------------------------------+  |
    //  |  |                                           |  |
    //  |  |           m_topContentVisual              |  |
    //  |  |                                           |  |
    //  +  +-------------------------------------------+  |
    //  |  +--------------------+ +--------------------+  |
    //  |  |                    | |                    |  |
    //  |  |  m_leftRootVisual  | | m_rightRootVisual  |  |
    //  |  |                    | |                    |  |
    //  |  +--------------------+ +--------------------+  |
    //  +-------------------------------------------------+

    // Create a helper object for setting visual positions.
    // The docker snaps to pixels unless we pass zero as the rasterization scale.
    float rasterizationScale = GetIsland().RasterizationScale();
    float dockerRasterizationScale = !GetSetting(Setting_DisablePixelSnapping) ? rasterizationScale : 0.0f;
    FrameDocker docker(GetIsland().ActualSize(), dockerRasterizationScale);

    // Add margins.
    docker.InsetTop(m_labelVisual.Size().Height);
    docker.InsetLeft(k_inset);
    docker.InsetRight(k_inset);
    docker.InsetBottom(k_inset);

    // Position the visuals.
    docker.DockTopRelativeWithMargin(m_topContentVisual, 0.5f, k_inset);
    docker.DockLeftRelativeWithMargin(m_leftRootVisual, 0.5f, k_inset);
    docker.DockFill(m_rightRootVisual);

    LiftedFrame::HandleContentLayout();

    if (nullptr != m_leftChildSiteLink)
    {
        auto leftContentVisualNode = m_leftContentPeer->VisualNode();
        m_leftChildSiteLink.LocalToParentTransformMatrix(leftContentVisualNode->Transform4x4());

        m_leftChildSiteLink.ActualSize(leftContentVisualNode->Size());
    }

    if (nullptr != m_rightChildSiteLink)
    {
        auto rightContentVisualNode = m_rightContentPeer->VisualNode();
        m_rightChildSiteLink.LocalToParentTransformMatrix(rightContentVisualNode->Transform4x4());

        m_rightChildSiteLink.ActualSize(rightContentVisualNode->Size());
    }
}
