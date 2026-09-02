// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "LiftedFrame.h"
#include "TextVisual.h"
#include "PreTranslateHandler.h"
#include "FocusManager.h"

class WinUIFrame final : public LiftedFrame
{
public:
    WinUIFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings);

    void ConnectPopupFrame(
        IFrame* frame);

    void OnPreTranslateTreeMessage(
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled) override;

private:
    void InitializeVisualTree(const winrt::Compositor& compositor);

    void InitializeInputEvents();

    void OnClick(
        const winrt::PointerPoint& point);

    void ActivateForPointer(
        const winrt::PointerPoint& point);

    void OnKeyPress(
        winrt::Windows::System::VirtualKey key);

    void OnActivationChanged(
        bool isActive);

    void HandleContentLayout() override;

private:
    static constexpr wchar_t k_frameName[] = L"WinUI";
    static constexpr float k_popupSize = 200.0f;
    static constexpr float k_layoutGap = 50.0f;

    LiftedTextVisual m_labelVisual;
    LiftedTextVisual m_acceleratorVisual;
    bool m_acceleratorActive = false;
    float m_labelDpiScale = 0.0f;

    IFrame* m_popupFrame = nullptr;
    winrt::DesktopPopupSiteBridge m_popupSiteBridge = nullptr;

    winrt::InputPointerSource m_pointerSource = nullptr;
    winrt::InputKeyboardSource m_keyboardSource = nullptr;
    winrt::InputPreTranslateKeyboardSource m_preTranslateKeyboardSource = nullptr;
    winrt::InputFocusController m_focusController = nullptr;
    winrt::com_ptr<PreTranslateHandler> m_preTranslateHandler{nullptr};
    FocusManager m_focusManager;
};
