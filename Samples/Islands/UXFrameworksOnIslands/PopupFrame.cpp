// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "PopupFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"

PopupFrame::PopupFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings) :
    LiftedFrame(compositor, settings),
    m_labelVisual(GetOutput(), k_frameName)
{
    m_labelVisual.SetBackgroundColor(ColorUtils::DarkRed());
    InitializeVisualTree(compositor);
    InitializeInputEvents();

    // Set up a handler for layout direction changes
    m_islandStateChangedRevoker = { GetIsland(), GetIsland().StateChanged([&](
        auto&&,  
        winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const& args)
    {
        if (args.DidLayoutDirectionChange())
        {
            // If inactive, use a more muted color
            auto color = ColorUtils::DarkRed();

            if (GetIsland().LayoutDirection() == winrt::ContentLayoutDirection::RightToLeft)
            {
                color = ColorUtils::LightRed();
            }

            auto outlineVisual = m_backgroundPeer->VisualNode()->Visual().as<winrt::SpriteVisual>();
            auto colorBrush = outlineVisual.Compositor().CreateColorBrush(color);
            outlineVisual.Brush(colorBrush);
        }
    }) };
}

void PopupFrame::ConnectPopupFrame(IFrame* frame)
{
    m_popupFrame = frame;
    m_popupSiteBridge = LiftedFrame::ConnectPopupFrame(frame);

    HandleContentLayout();
}

void PopupFrame::InitializeVisualTree(const winrt::Compositor& compositor)
{
    // Fill entire bounds with dark red color
    auto colorVisual = compositor.CreateSpriteVisual();
    auto colorVisualNode = VisualTreeNode::Create(colorVisual.as<::IUnknown>());
    m_backgroundPeer = m_automationTree->CreatePeer(colorVisualNode, k_frameName, UIA_ImageControlTypeId);
    GetRootVisualTreeNode()->AddChild(colorVisualNode);
    m_automationTree->Root()->AddChildToEnd(m_backgroundPeer->Fragment());
    m_automationTree->AddPeer(m_backgroundPeer);
    VisualUtils::FillWithColor(colorVisual, ColorUtils::DarkRed());

    // Insert the label into the tree.
    InsertTextVisual(m_labelVisual, m_automationTree, m_backgroundPeer);

    // This is just a dummy structural visual to hold the click squares
    auto clickSquareRoot = compositor.CreateContainerVisual();
    m_clickSquareRoot = VisualTreeNode::Create(clickSquareRoot.as<::IUnknown>());
    colorVisualNode->AddChild(m_clickSquareRoot);
}

void PopupFrame::InitializeInputEvents()
{
    auto island = GetIsland();
    m_pointerSource = winrt::InputPointerSource::GetForIsland(island);
    m_keyboardSource = winrt::InputKeyboardSource::GetForIsland(island);

    m_pointerSource.PointerPressed(
        [this](auto&& /*sender*/, auto&& args)
        {
            auto point = args.CurrentPoint();
            OnClick(point);
        });

    m_keyboardSource.KeyDown(
        [this](auto&& /*sender*/, auto&& args)
        {
            OnKeyPress(args.VirtualKey());
        });
}

void PopupFrame::OnClick(
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

    // In the RTL configuration, we need to adjust to the origin being in the top right corner
    if (GetIsland().LayoutDirection() == winrt::ContentLayoutDirection::RightToLeft)
    {
        blueVisual.Offset({GetIsland().ActualSize().x - (point.Position().X + 5.0f), point.Position().Y - 5.0f, 0.0});
    }
    else
    {
        blueVisual.Offset({point.Position().X - 5.0f, point.Position().Y - 5.0f, 0.0});
    }
}

void PopupFrame::OnKeyPress(
    winrt::Windows::System::VirtualKey key)
{
    if (key == winrt::Windows::System::VirtualKey::Escape)
    {
        m_popupSiteBridge.Hide();
    }
    else if (key == winrt::Windows::System::VirtualKey::Enter)
    {
        m_popupSiteBridge.Show();
    }
    else if (key == winrt::Windows::System::VirtualKey::R)
    {
        m_popupSiteBridge.LayoutDirectionOverride(winrt::ContentLayoutDirection::RightToLeft);
    }
    else if (key == winrt::Windows::System::VirtualKey::L)
    {
        m_popupSiteBridge.LayoutDirectionOverride(winrt::ContentLayoutDirection::LeftToRight);
    }
}

void PopupFrame::HandleContentLayout()
{
    if (m_popupSiteBridge != nullptr)
    {
        if (GetSetting(Setting_ShowPopupVisual))
        {
            auto islandSize = GetIsland().ActualSize();
            auto bounds = GetConverter().ConvertLocalToScreen({
                islandSize.x - k_layoutGap, 
                islandSize.y - k_layoutGap, 
                k_popupSize, 
                k_popupSize});

            m_popupSiteBridge.MoveAndResize(bounds);

            m_popupSiteBridge.Show();
        }
        else
        {
            m_popupSiteBridge.Hide();
        }
    }
}
