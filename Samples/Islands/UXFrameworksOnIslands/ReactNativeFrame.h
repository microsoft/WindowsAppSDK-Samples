// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "LiftedFrame.h"
#include "TextVisual.h"

class ReactNativeFrame final : public LiftedFrame, public IFrameHost
{
public:
    ReactNativeFrame(const winrt::Compositor& compositor, const std::shared_ptr<SettingCollection>& settings);

    void ConnectLeftFrame(
        IFrame* frame);
    void ConnectRightFrame(
        IFrame* frame);

    // IFrameHost overrides.
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetParentProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const sender) const override;

private:
    void InitializeVisualTree(
        const winrt::Compositor& compositor);

    void InitializeReactNativeContent();

    void RegisterForInputEvents();

    void OnClick(
        const winrt::PointerPoint& point);

    void ActivateForPointer(
        const winrt::PointerPoint& point);

    void OnKeyPress(
        winrt::Windows::System::VirtualKey key);

    void OnActivationChanged(
        bool isActive);

    void HandleContentLayout() override;

    void ActivateInteractionTracker(
        const winrt::Compositor& compositor, 
        const winrt::Visual& parentVisual, 
        const winrt::Visual& childVisual);

private:
    static constexpr wchar_t k_frameName[] = L"ReactNative";
    static constexpr float k_inset = 10.0f;
    static constexpr float k_size = 500.f;

    LiftedTextVisual m_labelVisual;
    std::shared_ptr<VisualTreeNode> m_clickSquareRoot = nullptr;

    winrt::ChildSiteLink m_leftChildSiteLink = nullptr;
    winrt::ChildSiteLink m_rightChildSiteLink = nullptr;

    IFrame* m_leftFrame = nullptr;
    IFrame* m_rightFrame = nullptr;

    winrt::ContainerVisual m_topContentVisual{nullptr};
    winrt::ContainerVisual m_leftRootVisual{nullptr};
    winrt::ContainerVisual m_rightRootVisual{nullptr};

    std::shared_ptr<AutomationPeer> m_backgroundPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_topContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_leftContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_rightContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_topColorPeer = nullptr;

    winrt::InputPointerSource m_pointerSource = nullptr;
    winrt::InputKeyboardSource m_keyboardSource = nullptr;
    winrt::InputActivationListener m_activationListener = nullptr;

    winrt::InteractionTracker m_interactionTracker{nullptr};
    winrt::VisualInteractionSource m_source{nullptr};
    winrt::ExpressionAnimation m_offsetAnimation{nullptr};
};
