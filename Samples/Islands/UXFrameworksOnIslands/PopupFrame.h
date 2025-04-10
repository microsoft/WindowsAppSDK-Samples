// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "LiftedFrame.h"
#include "TextVisual.h"

class PopupFrame final : public LiftedFrame
{
public:
    PopupFrame(
        const winrt::Compositor& compositor, 
        const std::shared_ptr<SettingCollection>& settings);

    ~PopupFrame() noexcept = default;

    void ConnectPopupFrame(
        IFrame* frame);

private:
    void InitializeVisualTree(const winrt::Compositor& compositor);

    void InitializeInputEvents();

    void OnClick(
        const winrt::PointerPoint& point);

    void OnKeyPress(
        winrt::Windows::System::VirtualKey key);

    void OnActivationChanged(
        bool isActive);

    void HandleContentLayout() override;

private:
    static constexpr wchar_t k_frameName[] = L"Popup";
    static constexpr float k_popupSize = 200.0f;
    static constexpr float k_layoutGap = 50.0f;

    LiftedTextVisual m_labelVisual;
    std::shared_ptr<VisualTreeNode> m_clickSquareRoot = nullptr;

    float m_labelDpiScale = 0.0f;

    IFrame* m_popupFrame = nullptr;
    winrt::DesktopPopupSiteBridge m_popupSiteBridge = nullptr;

    std::shared_ptr<AutomationPeer> m_backgroundPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_outlinePeer = nullptr;

    winrt::InputPointerSource m_pointerSource = nullptr;
    winrt::InputKeyboardSource m_keyboardSource = nullptr;
    winrt::WindowRectChangedEventArgs m_windowRectChanged = nullptr;

    ContentIslandStateChanged_revoker m_islandStateChangedRevoker{};
};
