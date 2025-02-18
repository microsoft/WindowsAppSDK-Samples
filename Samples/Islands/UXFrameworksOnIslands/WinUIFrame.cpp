// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "WinUIFrame.h"
#include "ColorUtils.h"
#include "VisualUtils.h"

WinUIFrame::WinUIFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings) :
    LiftedFrame(compositor, settings),
    m_labelVisual(GetOutput(), k_frameName),
    m_acceleratorVisual(GetOutput(), L"2"),
    m_focusManager(m_focusList)
{
    m_labelVisual.SetBackgroundColor(ColorUtils::LightYellow());
    InitializeVisualTree(compositor);
    
    InitializeInputEvents();
}

void WinUIFrame::ConnectPopupFrame(IFrame* frame)
{
    m_popupFrame = frame;
    m_popupSiteBridge = LiftedFrame::ConnectPopupFrame(frame);

    HandleContentLayout();
}

void WinUIFrame::InitializeInputEvents()
{
    auto island = GetIsland();
    m_preTranslateKeyboardSource = winrt::InputPreTranslateKeyboardSource::GetForIsland(island);
    m_pointerSource = winrt::InputPointerSource::GetForIsland(island);
    m_keyboardSource = winrt::InputKeyboardSource::GetForIsland(island);
    m_focusController = winrt::InputFocusController::GetForIsland(island);

    m_preTranslateHandler = winrt::make_self<PreTranslateHandler>(this);
    m_preTranslateKeyboardSource.as<winrt::Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop>()->SetPreTranslateHandler(m_preTranslateHandler.get());

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

    // FocusManager will listen to focus events.
    m_focusManager.InitializeWithFocusController(m_focusController);
}

void WinUIFrame::OnPreTranslateTreeMessage(
    const MSG* msg,
    UINT /*keyboardModifiers*/,
    _Inout_ bool* handled)
{
    *handled = false;

    if (msg->message == WM_SYSKEYDOWN || msg->message == WM_KEYDOWN)
    {
        if (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::Windows::System::VirtualKey::Control)
        {
            m_acceleratorActive = !m_acceleratorActive;
            m_acceleratorVisual.GetVisual().IsVisible(m_acceleratorActive);
        }
        else {
            m_acceleratorActive = false;
            m_acceleratorVisual.GetVisual().IsVisible(false);
            if (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::Windows::System::VirtualKey::Number2)
            {
                // Take focus
                m_focusController.TrySetFocus();
            }
        }
    }
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

    // Insert the accelerator label and make it invisible
    m_acceleratorVisual.GetVisual().Offset(winrt::Windows::Foundation::Numerics::float3(m_labelVisual.Size().Width+3, 0, 0));
    m_acceleratorVisual.GetVisual().IsVisible(false);
    InsertTextVisual(m_acceleratorVisual, m_automationTree, colorVisualPeer);
}

void WinUIFrame::OnClick(
    const winrt::PointerPoint& point)
{
    // Unconditionally take focus
    m_focusController.TrySetFocus();

    HitTestContext context{ m_focusManager, GetRootVisualTreeNode() };
    context.RunHitTest(point.Position(), point.Properties().IsRightButtonPressed());
}

void WinUIFrame::OnKeyPress(
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
    else if (key == winrt::Windows::System::VirtualKey::Tab)
    {
        // Key-state is a bitwise flag, check the "Down" flag specficially.
        auto shiftState = m_keyboardSource.GetKeyState(winrt::Windows::System::VirtualKey::Shift);
        bool isShiftDown = (shiftState & winrt::VirtualKeyStates::Down) != winrt::VirtualKeyStates::None;
        if (isShiftDown)
        {
            m_focusManager.NavigateBackward();
        }
        else
        {
            m_focusManager.NavigateForward();
        }
    }
}

void WinUIFrame::HandleContentLayout()
{
    LiftedFrame::HandleContentLayout();

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
