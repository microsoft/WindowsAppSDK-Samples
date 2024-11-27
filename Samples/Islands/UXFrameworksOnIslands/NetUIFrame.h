// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "SystemFrame.h"
#include "TextVisual.h"

class NetUIFrame final : public SystemFrame
{
public:
    NetUIFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor);

    void ConnectFrame(
        IFrame* frame);

    void ActivateForPointer(
        const winrt::Microsoft::UI::Input::PointerPoint& point);

protected:
    void InitializeVisualTree(
        const winrt::WUC::Compositor& compositor);

    void HandleContentLayout() override;

    void ActivateInteractionTracker(
        const winrt::WUC::Compositor& compositor, 
        const winrt::WUC::Visual& parentVisual, 
        const winrt::WUC::Visual& childVisual);

private:
    static constexpr wchar_t k_frameName[] = L"NetUI";
    static constexpr float k_inset = 10.0f;
    static constexpr float k_size = 500.f;

    std::shared_ptr<AutomationPeer> m_childContentPeer = nullptr;
    winrt::ChildContentLink m_childContentLink = nullptr;
    SystemTextVisual m_labelVisual;

    // Interaction tracker
    winrt::WUCI::InteractionTracker m_interactionTracker{nullptr};
    winrt::WUCI::VisualInteractionSource m_source{ nullptr };
    winrt::WUC::ExpressionAnimation m_offsetAnimation{ nullptr };

    winrt::InputPointerSource m_pointerSource = nullptr;
};
