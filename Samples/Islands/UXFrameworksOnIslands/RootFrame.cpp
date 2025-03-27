// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "RootFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"
#include "FrameDocker.h"
#include "TextRenderer.h"
#include "HitTestContext.h"

RootFrame::RootFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings) :
    SystemFrame(queue, systemCompositor, settings),
    m_rootLabel(GetOutput(), k_rootFrameName),
    m_acceleratorLabel(GetOutput(), L"1"),
    m_ribbonLabel(GetOutput(), k_ribbonFrameName, CreateTextLayout(k_ribbonFrameName, CreateTextFormat(L"Cambria", 24.0f).get())),
    m_backLabel(GetOutput(), L"Back"),
    m_frontLabel(GetOutput(), L"Front"),
    m_forceAliasedTextCheckBox(GetOutput(), L"Force aliased text", settings, Setting_ForceAliasedText),
    m_disablePixelSnappingCheckBox(GetOutput(), L"Disable pixel snapping", settings, Setting_DisablePixelSnapping),
    m_showSpriteBoundsCheckBox(GetOutput(), L"Show sprite bounds", settings, Setting_ShowSpriteBounds),
    m_showSpriteGenerationCheckBox(GetOutput(), L"Show sprite generation", settings, Setting_ShowSpriteGeneration),
    m_showPopupVisualCheckBox(GetOutput(), L"Show popups", settings, Setting_ShowPopupVisual),
    m_focusManager(m_focusList)
{
    InitializeVisualTree(systemCompositor);

    auto islandEnvironment = GetIsland().Environment();
    m_islandEnvironmentStateChangedRevoker = { islandEnvironment, islandEnvironment.StateChanged(
        [&](auto&&, winrt::Microsoft::UI::Content::ContentEnvironmentStateChangedEventArgs const& args)
        {
            if (args.DidDisplayScaleChange())
            {
                HandleDisplayScaleChanged();
            }
        }) 
    };
}

bool RootFrame::SystemPreTranslateMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if (message == WM_SYSKEYDOWN || message == WM_KEYDOWN)
    {
        if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Menu)
        {
            m_acceleratorActive = !m_acceleratorActive;
            m_acceleratorLabel.GetVisual().IsVisible(m_acceleratorActive);
        }
        else {
            m_acceleratorActive = false;
            m_acceleratorLabel.GetVisual().IsVisible(false);
            if (static_cast<winrt::Windows::System::VirtualKey>(wParam) == winrt::Windows::System::VirtualKey::Number1)
            {
                // Take focus
                m_focusManager.SetFocusToVisual(GetRootVisualTreeNode(), GetRootVisualTreeNode()->OwningFocusList());
            }
        }
    }

    if (m_leftFrame->SystemPreTranslateMessage(message, wParam, lParam))
    {
        return true;
    }
    if (m_rightFrame->SystemPreTranslateMessage(message, wParam, lParam))
    {
        return true;
    }
    return false;
}

LRESULT RootFrame::HandleMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    _Out_ bool* handled)
{
    UNREFERENCED_PARAMETER(wParam);

    *handled = false;
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            OnClick(PointFromLParam(lParam), false);
            *handled = true;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            OnClick(PointFromLParam(lParam), true);
            *handled = true;
            break;
        }
        case WM_KEYDOWN:
        {
            OnKeyPress(wParam);
            *handled = true;
            break;
        }
        case WM_SETFOCUS:
        {
            m_focusManager.RestoreFocus();
            break;
        }
        case WM_KILLFOCUS:
        {
            m_focusManager.ClearFocus();
            break;
        }
    }

    return 0;
}

void RootFrame::ConnectLeftFrame(IFrame* frame)
{
    m_leftFrame = frame;
    auto leftContentVisualNode = m_leftContentPeer->VisualNode();
    m_leftChildSiteLink = ConnectChildFrame(frame, leftContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_leftContentPeer->SetChildSiteLink(m_leftChildSiteLink);

        // Focus flows through the InputFocusNavigationHost API when the child is lifted content.
        m_focusList->SetChildSiteLink(m_rightChildSiteLink, 2);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_leftContentPeer->SetChildFrame(frame);

        // Set up hit testing so these visuals are treated as parent / child in the hit test tree.
        // This is needed because a child system island does not implicitly get pointer events and
        // needs to have hit testing forwarded by the parent island.
        HitTestContext::ConfigureCrossTreeConnection(leftContentVisualNode, frame->GetRootVisualTreeNode());

        // Focus lists are directly linked together for a child system frame.
        m_focusList->SetChildFocusList(frame->GetFocusList(), 2);
    }

    HandleContentLayout();
}

void RootFrame::ConnectRightFrame(IFrame* frame)
{
    m_rightFrame = frame;
    auto rightContentVisualNode = m_rightContentPeer->VisualNode();
    m_rightChildSiteLink = ConnectChildFrame(frame, rightContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_rightContentPeer->SetChildSiteLink(m_rightChildSiteLink);

        // Focus flows through Content APIs when the child is lifted content.
        m_focusList->SetChildSiteLink(m_rightChildSiteLink, 3);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_rightContentPeer->SetChildFrame(frame);

        // Set up hit testing so these visuals are treated as parent / child in the hit test tree.
        // This is needed because a child system island does not implicitly get pointer events and
        // needs to have hit testing forwarded by the parent island.
        HitTestContext::ConfigureCrossTreeConnection(rightContentVisualNode, frame->GetRootVisualTreeNode());

        // Focus lists are directly linked together for a system frame.
        m_focusList->SetChildFocusList(frame->GetFocusList(), 3);
    }

    HandleContentLayout();
}

winrt::com_ptr<::IRawElementProviderFragmentRoot> RootFrame::GetFragmentRootProviderForChildFrame(_In_ IFrame const* const) const
{
    return m_frameHost->GetFragmentRootProviderForChildFrame(this);
}

winrt::com_ptr<::IRawElementProviderFragment> RootFrame::GetNextSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // We've set up our automation tree in a way so that the frame providers are solitary children of their parent.
    return nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> RootFrame::GetParentProviderForChildFrame(_In_ IFrame const* const sender) const
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

winrt::com_ptr<::IRawElementProviderFragment> RootFrame::GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // We've set up our automation tree in a way so that the frame providers are solitary children of their parent.
    return nullptr;
}

void RootFrame::SetFocusHost(IFocusHost* focusHost)
{
    m_focusManager.InitializeWithFocusHost(focusHost);
}

winrt::Point RootFrame::PointFromLParam(
    LPARAM lParam) const
{
    auto rasterizationScale = GetIsland().RasterizationScale();

    return { 
        static_cast<float>(GET_X_LPARAM(lParam)) / rasterizationScale,
        static_cast<float>(GET_Y_LPARAM(lParam)) / rasterizationScale
    };
}

void RootFrame::InitializeVisualTree(
    const winrt::WUC::Compositor& compositor)
{
    auto rootVisual = compositor.CreateSpriteVisual();
    auto rootVisualNode = VisualTreeNode::Create(rootVisual.as<::IUnknown>());
    rootVisualNode->HitTestCallback([this](const HitTestContext& context) {
        AddClickSquare(context.Point(), context.IsRightClick());
        return true;
    });

    auto rootVisualPeer = m_automationTree->CreatePeer(rootVisualNode, k_rootFrameName, UIA_PaneControlTypeId);
    GetRootVisualTreeNode()->AddChild(rootVisualNode);
    m_automationTree->Root()->AddChildToEnd(rootVisualPeer->Fragment());
    m_automationTree->AddPeer(rootVisualPeer);

    rootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });

    // Set our background color
    auto colorBrush = compositor.CreateColorBrush(ColorUtils::LightPink());
    rootVisual.Brush(colorBrush);

    // Partition the layout into two regions: ribbon and document content
    m_ribbonRootVisual = compositor.CreateContainerVisual();
    auto ribbonRootVisualNode = VisualTreeNode::Create(m_ribbonRootVisual.as<::IUnknown>());
    m_ribbonPeer = m_automationTree->CreatePeer(ribbonRootVisualNode, k_ribbonFrameName, UIA_PaneControlTypeId);
    rootVisualNode->AddChild(ribbonRootVisualNode);
    rootVisualPeer->Fragment()->AddChildToEnd(m_ribbonPeer->Fragment());
    m_automationTree->AddPeer(m_ribbonPeer);

    m_documentRootVisual = compositor.CreateContainerVisual();
    auto documentRootVisualNode = VisualTreeNode::Create(m_documentRootVisual.as<::IUnknown>());
    m_documentPeer = m_automationTree->CreatePeer(documentRootVisualNode, k_documentFrameName, UIA_PaneControlTypeId);
    rootVisualNode->AddChild(documentRootVisualNode);
    rootVisualPeer->Fragment()->AddChildToEnd(m_documentPeer->Fragment());
    m_automationTree->AddPeer(m_documentPeer);

    InitializeRibbonContent();
    InitializeDocumentContent();

    // Insert the labels into the tree.
    InsertTextVisual(m_rootLabel, m_automationTree, rootVisualPeer);
    InsertTextVisual(m_ribbonLabel, m_automationTree, m_ribbonContentPeer);
    m_acceleratorLabel.SetBackgroundColor(winrt::Microsoft::UI::Colors::OldLace());
    m_acceleratorLabel.GetVisual().Offset(winrt::Windows::Foundation::Numerics::float3(m_rootLabel.Size().Width+3, 0, 0));
    m_acceleratorLabel.GetVisual().IsVisible(false);
    InsertTextVisual(m_acceleratorLabel, m_automationTree, rootVisualPeer);

    // Rotate the ribbon label 22 degrees and re-rasterize.
    m_ribbonLabel.GetVisual().RotationAngleInDegrees(22);

    // On top of all of them is the click square content
    auto clickSquareRoot = compositor.CreateContainerVisual();
    m_clickSquareRoot = VisualTreeNode::Create(clickSquareRoot.as<::IUnknown>());
    rootVisualNode->AddChild(m_clickSquareRoot);

    // Setup our focus list
    m_focusList->AddVisual(m_ribbonContentPeer->VisualNode()); // Index 1 (root visual is always 0)
    m_focusList->AddPlaceholder(); // Left content - index 2
    m_focusList->AddPlaceholder(); // Right content - index 3
}

void RootFrame::InitializeDocumentContent()
{
    // We make 4 pieces of content here: back, front, left, and right
    // 'back' and 'front' sit, respectively, above and below in z-order from the left and right content
    // The left and right content are set up to be child frames.
    // Everything is slightly overlapping to make things interesting and make clear
    // what the effects of the z-ordering are.
    auto documentRootVisualNode = m_documentPeer->VisualNode();
    auto documentFragment = m_documentPeer->Fragment();

    auto compositor = m_documentRootVisual.Compositor();
    auto grayBrush = compositor.CreateColorBrush(winrt::Windows::UI::Colors::DarkGray());

    // Helper to add the "Back" and "Front" visuals.
    auto AddLabeledRectangle = [&](SystemTextVisual& label) {
        auto visual = compositor.CreateContainerVisual();

        auto visualNode = VisualTreeNode::Create(visual.as<::IUnknown>());
        auto visualPeer = m_automationTree->CreatePeer(visualNode, label.GetText(), UIA_ImageControlTypeId);
        documentRootVisualNode->AddChild(visualNode);
        documentFragment->AddChildToEnd(visualPeer->Fragment());
        m_automationTree->AddPeer(visualPeer);

        auto colorVisual = compositor.CreateSpriteVisual();
        auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
        auto colorVisualPeer = m_automationTree->CreatePeer(colorVisualNode, label.GetText(), UIA_ImageControlTypeId);
        visualNode->AddChild(colorVisualNode);
        colorVisual.RelativeSizeAdjustment({1.0f, 1.0f});
        colorVisual.Brush(grayBrush);

        InsertTextVisual(label, m_automationTree, visualPeer);

        return visual;
    };

    // Back content
    m_backContentVisual = AddLabeledRectangle(m_backLabel);

    // Left Frame content
    m_leftContentVisual = compositor.CreateContainerVisual();
    auto leftContentVisualNode = VisualTreeNode::Create(m_leftContentVisual.as<::IUnknown>());
    m_leftContentPeer = m_automationTree->CreatePeer(leftContentVisualNode, L"Left", UIA_PaneControlTypeId);
    documentRootVisualNode->AddChild(leftContentVisualNode);
    documentFragment->AddChildToEnd(m_leftContentPeer->Fragment());
    m_automationTree->AddPeer(m_leftContentPeer);

    // Right Frame content
    m_rightContentVisual = compositor.CreateContainerVisual();
    auto rightContentVisualNode = VisualTreeNode::Create(m_rightContentVisual.as<::IUnknown>());
    m_rightContentPeer = m_automationTree->CreatePeer(rightContentVisualNode, L"Right", UIA_PaneControlTypeId);
    documentRootVisualNode->AddChild(rightContentVisualNode);
    documentFragment->AddChildToEnd(m_rightContentPeer->Fragment());
    m_automationTree->AddPeer(m_rightContentPeer);

    // Front content
    m_frontContentVisual = AddLabeledRectangle(m_frontLabel);
}

void RootFrame::InitializeRibbonContent()
{
    // Eventually we want to make the ribbon content more complicated,
    // but for now we just make a slightly darker pink rectangle
    auto ribbonRootVisualNode = m_ribbonPeer->VisualNode();
    auto ribbonRoot = ribbonRootVisualNode->Visual().as<winrt::WUC::ContainerVisual>();
    auto ribbonFragment = m_ribbonPeer->Fragment();

    auto compositor = ribbonRoot.Compositor();
    auto ribbonBrush = compositor.CreateColorBrush(ColorUtils::DarkPink());

    m_ribbonContentVisual = compositor.CreateSpriteVisual();
    auto ribbonContentVisualNode = VisualTreeNode::Create(m_ribbonContentVisual.as<::IUnknown>());
    m_ribbonContentPeer = m_automationTree->CreatePeer(ribbonContentVisualNode, L"Ribbon Content", UIA_ImageControlTypeId);
    ribbonRootVisualNode->AddChild(ribbonContentVisualNode);
    ribbonFragment->AddChildToEnd(m_ribbonContentPeer->Fragment());
    m_automationTree->AddPeer(m_ribbonContentPeer);
    m_ribbonContentVisual.Brush(ribbonBrush);

    InsertCheckBoxVisual(m_forceAliasedTextCheckBox, m_automationTree, m_ribbonContentPeer);
    InsertCheckBoxVisual(m_disablePixelSnappingCheckBox, m_automationTree, m_ribbonContentPeer);
    InsertCheckBoxVisual(m_showSpriteBoundsCheckBox, m_automationTree, m_ribbonContentPeer);
    InsertCheckBoxVisual(m_showSpriteGenerationCheckBox, m_automationTree, m_ribbonContentPeer);
    InsertCheckBoxVisual(m_showPopupVisualCheckBox, m_automationTree, m_ribbonContentPeer);
}

void RootFrame::OnClick(
    const winrt::Point& point,
    bool isRightClick)
{
    // Run a hittest. This will invoke any callbacks on the visual tree nodes.
    HitTestContext context{ m_focusManager, GetRootVisualTreeNode() };
    context.RunHitTest(point, isRightClick);
}

void RootFrame::AddClickSquare(
    const winrt::Point& point,
    bool isRightClick)
{
    auto compositor = m_clickSquareRoot->Visual().as<winrt::WUC::ContainerVisual>().Compositor();

    // Create a 10x10 visual
    auto visual = compositor.CreateSpriteVisual();
    auto visualNode = VisualTreeNode::Create(visual.as<::IUnknown>());
    m_clickSquareRoot->AddChild(visualNode);
    visual.Size({ 10.0f, 10.0f });
    visual.Offset({ point.X - 5, point.Y - 5, 0.0f });

    // Set the brush color based on isRightClick
    auto color = isRightClick ? winrt::Windows::UI::Colors::DeepPink() : winrt::Windows::UI::Colors::Orange();
    auto brush = compositor.CreateColorBrush(color);
    visual.Brush(brush);
}

void RootFrame::OnKeyPress(
    WPARAM wParam)
{
    if (wParam == 'A' || wParam == 'B' || wParam == 'C')
    {
        auto compositor = GetRootVisualTreeNode()->Visual().as<winrt::WUC::Visual>().Compositor();
        winrt::Windows::UI::Color color =
            (wParam == 'A') ? ColorUtils::DarkPink() : 
            (wParam == 'B') ? winrt::Windows::UI::Colors::Purple() :
            winrt::Windows::UI::Colors::Lavender();
        auto brush = compositor.CreateColorBrush(color);
        m_ribbonContentPeer->VisualNode()->Visual().as<winrt::WUC::SpriteVisual>().Brush(brush);
    }
    else if (wParam == VK_ESCAPE)
    {
        m_clickSquareRoot->RemoveAllChildren();
    }
    else if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS)
    {
        SetZoomFactor(m_zoomFactor - k_zoomIncrement);
    }
    else if (wParam == VK_ADD || wParam == VK_OEM_PLUS)
    {
        SetZoomFactor(m_zoomFactor + k_zoomIncrement);
    }
    else if (wParam == 'O')
    {
        m_rotationAngle = (m_rotationAngle + (m_rotationAngleLimit - 1)) % m_rotationAngleLimit;
        HandleContentLayout();
    }
    else if (wParam == 'P')
    {
        m_rotationAngle = (m_rotationAngle + 1) % m_rotationAngleLimit;
        HandleContentLayout();
    }
    else if (wParam == VK_TAB)
    {
        if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
        {
            m_focusManager.NavigateBackward();
        }
        else
        {
            m_focusManager.NavigateForward();
        }
    }
}

void RootFrame::SetZoomFactor(float newZoom)
{
    newZoom = std::max(k_minZoom, std::min(k_maxZoom, newZoom));
    if (newZoom != m_zoomFactor)
    {
        m_zoomFactor = newZoom;
        if (m_leftChildSiteLink != nullptr)
        {
            HandleContentLayout();
        }
    }
}

void RootFrame::HandleContentLayout()
{
    // Visual tree:
    //
    //  root
    //    |
    //    +---- m_ribbonRootVisual
    //    |
    //    +---- m_documentRootVisual
    //              |
    //              +---- m_backContentVisual
    //              |
    //              +---- m_leftContentVisual
    //              |
    //              +---- m_rightContentVisual
    //              |
    //              +---- m_frontContentVisual
    //
    // Visual layout:
    //
    //  +---------------------------------------------------+
    //  |               m_ribbonRootVisual                  |
    //  +------------------------+-+------------------------+
    //  |                        | |                        |
    //  |  m_leftContentVisual   | |  m_rightContentVisual  |
    //  |                        | |                        |
    //  |                m_backContentVisual                |
    //  +----------------m_frontContentVisual---------------+
    //  +---------------------------------------------------+
    //  

    // The root island's rasterization scale is 1, so we will explicitily multiply
    // logical units by the display scale.
    const float rasterizationScale = GetIsland().RasterizationScale();
    const float displayScale = GetIsland().Environment().DisplayScale();
    assert(rasterizationScale == 1);

    // Ensure each label's scale matches the display scale.
    if (m_rootLabel.GetVisual().Scale().x != displayScale)
    {
        for (auto* label : { &m_rootLabel, &m_ribbonLabel, &m_backLabel, &m_frontLabel })
        {
            label->GetVisual().Scale({displayScale, displayScale, 1.0f});
        }

        for (auto* control : { &m_forceAliasedTextCheckBox, &m_disablePixelSnappingCheckBox, &m_showSpriteBoundsCheckBox, &m_showSpriteGenerationCheckBox, &m_showPopupVisualCheckBox })
        {
            control->GetVisual().Scale({displayScale, displayScale, 1.0f});
        }
    }

    // Create a helper object for setting visual positions.
    // The docker snaps to pixels unless we pass zero as the rasterization scale.
    float dockerRasterizationScale = !GetSetting(Setting_DisablePixelSnapping) ? rasterizationScale : 0.0f;
    FrameDocker docker(GetIsland().ActualSize(), dockerRasterizationScale);

    // Reserve whitespace around the boundary of the island.
    float inset = k_inset * displayScale;
    docker.Inset(inset);

    // Dock the ribbon root visual to the top of the island.
    docker.DockTop(m_ribbonRootVisual, k_ribbonHeight * displayScale);

    // Use relative layout to position the ribbon content within the ribbon root.
    m_ribbonContentVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_ribbonContentVisual.Size({ -(inset*2), -(inset*2) });
    m_ribbonContentVisual.Offset({ inset, inset, 0.0f });

    // Position the check boxes within the ribbon.
    float checkBoxLeft = 100.0f * displayScale;
    for (auto* control : { &m_forceAliasedTextCheckBox, &m_disablePixelSnappingCheckBox, &m_showSpriteBoundsCheckBox, &m_showSpriteGenerationCheckBox, &m_showPopupVisualCheckBox })
    {
        auto& visual = control->GetVisual();
        visual.Offset({ checkBoxLeft, inset * 2, 0.0f });
        checkBoxLeft += (visual.Size().x + 20.0f) * displayScale;
    }

    // Let the document root visual fill the remaining space.
    docker.DockFill(m_documentRootVisual);

    // The remaining visuals are children of the document root visual.
    docker = FrameDocker(m_documentRootVisual.Size(), dockerRasterizationScale);

    // Position the rectangular "back" and "front" visuals.
    float rectWidth = 100 * displayScale;
    float rectHeight = 200 * displayScale;
    m_backContentVisual.Offset({docker.Left(), docker.Top(), 0.0f});
    m_backContentVisual.Size({rectWidth, rectHeight});
    m_frontContentVisual.Offset({docker.Right() - rectWidth, docker.Bottom() - rectHeight, 0.0f});
    m_frontContentVisual.Size({rectWidth, rectHeight});

    // Add some whitespace so the left and right content visuals only partially overlap
    // the "back" and "front" visuals.
    docker.InsetTop(m_backLabel.Size().Height * displayScale);
    docker.InsetLeft(inset);
    docker.InsetRight(inset * 2);

    // Position the left and right content visuals.
    docker.DockLeftRelativeWithMargin(m_leftContentVisual, 0.5f, inset);
    docker.DockFill(m_rightContentVisual);

    // Scale the left and right content visuals, so the child islands' rasterization scale
    // will include DPI scaling. The left visual's scale also includes zoom.
    m_leftContentVisual.Scale({displayScale * m_zoomFactor, displayScale * m_zoomFactor, 1.0f});
    m_rightContentVisual.Scale({displayScale, displayScale, 1.0f});

    // Divide both visuals' sizes by the display scale because the size is in pre-scaled units.
    m_leftContentVisual.Size(m_leftContentVisual.Size() / displayScale);
    m_rightContentVisual.Size(m_rightContentVisual.Size() / displayScale);

    // Set the left content visual's rotatoin angle.
    m_leftContentVisual.RotationAngle(m_rotationAngle * m_rotationAngleUnit);

    SystemFrame::HandleContentLayout();

    if (m_leftChildSiteLink != nullptr)
    {
        auto leftContentVisualNode = m_leftContentPeer->VisualNode();
        m_leftChildSiteLink.LocalToParentTransformMatrix(leftContentVisualNode->Transform4x4());

        m_leftChildSiteLink.ActualSize(leftContentVisualNode->Size());
    }

    if (m_rightChildSiteLink != nullptr)
    {
        auto rightContentVisualNode = m_rightContentPeer->VisualNode();
        m_rightChildSiteLink.LocalToParentTransformMatrix(rightContentVisualNode->Transform4x4());

        m_rightChildSiteLink.ActualSize(rightContentVisualNode->Size());
    }
}

void RootFrame::HandleDisplayScaleChanged()
{
    HandleContentLayout();
}
