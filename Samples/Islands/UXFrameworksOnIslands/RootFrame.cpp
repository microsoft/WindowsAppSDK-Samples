// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "RootFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"
#include "TextRenderer.h"

RootFrame::RootFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor) :
    SystemFrame(compositor, systemCompositor),
    m_rootLabel(
        GetOutput(),
        /*text*/ k_rootFrameName,
        /*backgroundColor*/ winrt::Windows::UI::Colors::Transparent(),
        /*textColor*/ winrt::Windows::UI::ColorHelper::FromArgb(0x80, 0xFF, 0xFF, 0xFF),
        CreateTextFormat(L"Gil Sans Nova", 16.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC).get()),
    m_ribbonLabel(
        GetOutput(),
        /*text*/ k_ribbonFrameName,
        /*backgroundColor*/ winrt::Windows::UI::Colors::Transparent(),
        /*textColor*/ winrt::Windows::UI::ColorHelper::FromArgb(0x80, 0xFF, 0xFF, 0xFF),
        CreateTextFormat(L"Gil Sans Nova", 24.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC).get())
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
        {    OnKeyPress(wParam);
            *handled = true;
            break;
        }
    }

    return 0;
}

void RootFrame::ConnectLeftFrame(IFrame* frame)
{
    m_leftFrame = frame;
    auto leftContentVisualNode = m_leftContentPeer->VisualNode();
    m_leftChildLink = ConnectChildFrame(frame, leftContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_leftContentPeer->SetChildContentLink(m_leftChildLink);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_leftContentPeer->SetChildFrame(frame);
    }

    HandleContentLayout();
}

void RootFrame::ConnectRightFrame(IFrame* frame)
{
    m_rightFrame = frame;
    auto rightContentVisualNode = m_rightContentPeer->VisualNode();
    m_rightChildLink = ConnectChildFrame(frame, rightContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>());

    if (frame->IsLiftedFrame())
    {
        // Automation flows through the Content APIs when the child is lifted content.
        m_rightContentPeer->SetChildContentLink(m_rightChildLink);
    }
    else
    {
        // Automation flows through an explicit IFrameHost API when the child is system content.
        m_rightContentPeer->SetChildFrame(frame);
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
    auto rootVisualPeer = m_automationTree->CreatePeer(rootVisualNode, k_rootFrameName, UIA_PaneControlTypeId);
    GetRootVisualTreeNode()->AddChild(rootVisualNode);
    m_automationTree->Root()->AddChildToEnd(rootVisualPeer->Fragment());
    m_automationTree->AddPeer(rootVisualPeer);

    rootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });

    // Set our background color
    auto colorBrush = compositor.CreateColorBrush(ColorUtils::LightPink());
    rootVisual.Brush(colorBrush);

    // Partition the layout into two regions: ribbon and document content
    auto ribbonRoot = compositor.CreateContainerVisual();
    auto ribbonRootVisualNode = VisualTreeNode::Create(ribbonRoot.as<::IUnknown>());
    m_ribbonPeer = m_automationTree->CreatePeer(ribbonRootVisualNode, k_ribbonFrameName, UIA_PaneControlTypeId);
    rootVisualNode->AddChild(ribbonRootVisualNode);
    rootVisualPeer->Fragment()->AddChildToEnd(m_ribbonPeer->Fragment());
    m_automationTree->AddPeer(m_ribbonPeer);

    auto documentRoot = compositor.CreateContainerVisual();
    auto documentRootVisualNode = VisualTreeNode::Create(documentRoot.as<::IUnknown>());
    m_documentPeer = m_automationTree->CreatePeer(documentRootVisualNode, k_documentFrameName, UIA_PaneControlTypeId);
    rootVisualNode->AddChild(documentRootVisualNode);
    rootVisualPeer->Fragment()->AddChildToEnd(m_documentPeer->Fragment());
    m_automationTree->AddPeer(m_documentPeer);

    // Ribbon content is contained to the top of the frame
    VisualUtils::LayoutAsFillTop(ribbonRoot, 0.20f);
    VisualUtils::LayoutAsInset(ribbonRoot, k_inset);

    // Document content is contained to the bottom 2/3 of the frame
    VisualUtils::LayoutAsFillBottom(documentRoot, 0.8f);
    VisualUtils::LayoutAsInset(documentRoot, k_inset);

    InitializeRibbonContent();
    InitializeDocumentContent();

    // Insert the labels into the tree.
    InsertTextVisual(m_rootLabel, m_automationTree, rootVisualPeer);
    InsertTextVisual(m_ribbonLabel, m_automationTree, m_ribbonContentPeer);

    // Rotate the ribbon label 22 degrees and re-rasterize.
    m_ribbonLabel.GetVisual().RotationAngleInDegrees(22);
    m_ribbonLabel.HandleRasterizationScaleChanged(GetOutput());

    // On top of all of them is the click square content
    auto clickSquareRoot = compositor.CreateContainerVisual();
    m_clickSquareRoot = VisualTreeNode::Create(clickSquareRoot.as<::IUnknown>());
    rootVisualNode->AddChild(m_clickSquareRoot);
}

void RootFrame::InitializeDocumentContent()
{
    // We make 4 pieces of content here: above, below, left, and right
    // 'above' and 'below' sit, respectively, above and below in z-order from the left and right content
    // The left and right content are set up to be child frames.
    // Everything is slightly overlapping to make things interesting and make clear
    // what the effects of the z-ordering are.
    auto documentRootVisualNode = m_documentPeer->VisualNode();
    auto documentRoot = documentRootVisualNode->Visual().as<winrt::WUC::ContainerVisual>();
    auto documentFragment = m_documentPeer->Fragment();

    auto compositor = documentRoot.Compositor();
    auto grayBrush = compositor.CreateColorBrush(winrt::Windows::UI::Colors::DarkGray());

    // Below content
    auto belowContent = compositor.CreateSpriteVisual();
    auto belowContentVisualNode = VisualTreeNode::Create(belowContent.as<::IUnknown>());
    auto belowContentPeer = m_automationTree->CreatePeer(belowContentVisualNode, L"Below", UIA_ImageControlTypeId);
    documentRootVisualNode->AddChild(belowContentVisualNode);
    documentFragment->AddChildToEnd(belowContentPeer->Fragment());
    m_automationTree->AddPeer(belowContentPeer);
    VisualUtils::LayoutAsFillTop(belowContent, 0.5f);
    VisualUtils::LayoutAsFillLeft(belowContent, 0.125f);
    belowContent.Brush(grayBrush);

    // Left Frame content
    auto leftContent = compositor.CreateContainerVisual();
    auto leftContentVisualNode = VisualTreeNode::Create(leftContent.as<::IUnknown>());
    m_leftContentPeer = m_automationTree->CreatePeer(leftContentVisualNode, L"Left", UIA_PaneControlTypeId);
    documentRootVisualNode->AddChild(leftContentVisualNode);
    documentFragment->AddChildToEnd(m_leftContentPeer->Fragment());
    m_automationTree->AddPeer(m_leftContentPeer);
    auto displayScale = GetIsland().Environment().DisplayScale();
    leftContent.Scale({ displayScale, displayScale, 1.0f });
    leftContent.Offset({ k_inset, k_inset, 0.0f });
    leftContent.Size({ 500.0f, 500.0f });

    // Right Frame content
    auto rightContent = compositor.CreateContainerVisual();
    auto rightContentVisualNode = VisualTreeNode::Create(rightContent.as<::IUnknown>());
    m_rightContentPeer = m_automationTree->CreatePeer(rightContentVisualNode, L"Right", UIA_PaneControlTypeId);
    documentRootVisualNode->AddChild(rightContentVisualNode);
    documentFragment->AddChildToEnd(m_rightContentPeer->Fragment());
    m_automationTree->AddPeer(m_rightContentPeer);
    rightContent.Scale({ displayScale, displayScale, 1.0f });
    rightContent.RelativeOffsetAdjustment({ 0.5f, 0.0f, 0.0f });
    rightContent.Offset({ -k_inset, k_inset * 2, 0.0f });
    rightContent.Size({ 500.0f, 500.0f });

    // Above content
    auto aboveContent = compositor.CreateSpriteVisual();
    auto aboveContentVisualNode = VisualTreeNode::Create(aboveContent.as<::IUnknown>());
    auto aboveContentPeer = m_automationTree->CreatePeer(aboveContentVisualNode, L"Above", UIA_ImageControlTypeId);
    documentRootVisualNode->AddChild(aboveContentVisualNode);
    documentFragment->AddChildToEnd(aboveContentPeer->Fragment());
    m_automationTree->AddPeer(aboveContentPeer);
    VisualUtils::LayoutAsFillBottom(aboveContent, 0.5f);
    VisualUtils::LayoutAsFillRight(aboveContent, 0.125f);
    aboveContent.Brush(grayBrush);
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

    auto ribbonContentVisual = compositor.CreateSpriteVisual();
    auto ribbonContentVisualNode = VisualTreeNode::Create(ribbonContentVisual.as<::IUnknown>());
    m_ribbonContentPeer = m_automationTree->CreatePeer(ribbonContentVisualNode, L"Ribbon Content", UIA_ImageControlTypeId);
    ribbonRootVisualNode->AddChild(ribbonContentVisualNode);
    ribbonFragment->AddChildToEnd(m_ribbonContentPeer->Fragment());
    m_automationTree->AddPeer(m_ribbonContentPeer);
    ribbonContentVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    ribbonContentVisual.Size({ -(k_inset*2), -(k_inset*2) });
    ribbonContentVisual.Offset({ k_inset, k_inset, 0.0f });
    ribbonContentVisual.Brush(ribbonBrush);
}


void RootFrame::OnClick(
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
    else if (wParam == VK_SUBTRACT)
    {
        if (m_leftChildLink != nullptr)
        {
            auto leftContentVisualNode = m_leftContentPeer->VisualNode();
            auto leftContentVisual = leftContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>();
            
            auto newScale = leftContentVisual.Scale().x - k_scaleIncrement;
            if (newScale >= k_minScale)
            {
                leftContentVisual.Scale({newScale, newScale, 1.0});
                HandleContentLayout();
            }            
        }
    }
    else if (wParam == VK_ADD)
    {
        if (m_leftChildLink != nullptr)
        {
            auto leftContentVisualNode = m_leftContentPeer->VisualNode();
            auto leftContentVisual = leftContentVisualNode->Visual().as<winrt::WUC::ContainerVisual>();

            auto newScale = leftContentVisual.Scale().x + k_scaleIncrement;
            if (newScale <= k_maxScale)
            {
                leftContentVisual.Scale({newScale, newScale, 1.0});
                HandleContentLayout();
            }         
        }
    }
    else if (wParam == 'O')
    {
        if (m_leftChildLink != nullptr)
        {
            auto leftContentVisual = m_leftContentPeer->VisualNode()->Visual().as<winrt::WUC::ContainerVisual>();

            auto newRotation = leftContentVisual.RotationAngleInDegrees() - k_rotationIncrement;
            if (newRotation >= k_minRotation)
            {
                leftContentVisual.RotationAngleInDegrees(newRotation);
                HandleContentLayout();
            }
        }
    }
    else if (wParam == 'P')
    {
        if (m_leftChildLink != nullptr)
        {
            auto leftContentVisual = m_leftContentPeer->VisualNode()->Visual().as<winrt::WUC::ContainerVisual>();

            auto newRotation = leftContentVisual.RotationAngleInDegrees() + k_rotationIncrement;
            if (newRotation <= k_maxRotation)
            {                
                leftContentVisual.RotationAngleInDegrees(newRotation);
                HandleContentLayout();
            }
        }
    }
}

void RootFrame::HandleContentLayout()
{
    SystemFrame::HandleContentLayout();

    if (m_leftChildLink != nullptr)
    {
        auto leftContentVisualNode = m_leftContentPeer->VisualNode();
        m_leftChildLink.TransformMatrix(leftContentVisualNode->Transform3x2());

        m_leftChildLink.ActualSize(leftContentVisualNode->Size());
    }

    if (m_rightChildLink != nullptr)
    {
        auto rightContentVisualNode = m_rightContentPeer->VisualNode();
        m_rightChildLink.TransformMatrix(rightContentVisualNode->Transform3x2());

        m_rightChildLink.ActualSize(rightContentVisualNode->Size());
    }
}

void RootFrame::HandleDisplayScaleChanged()
{
    auto displayScale = GetIsland().Environment().DisplayScale();

    if (m_leftContentPeer != nullptr)
    {
        auto leftContent = m_leftContentPeer->VisualNode()->Visual().as<winrt::WUC::Visual>();
        leftContent.Scale({ displayScale, displayScale, 1.0f });
    }

    if (m_rightContentPeer != nullptr)
    {
        auto rightContent = m_rightContentPeer->VisualNode()->Visual().as<winrt::WUC::Visual>();
        rightContent.Scale({ displayScale, displayScale, 1.0f });
    }

    HandleContentLayout();
}
