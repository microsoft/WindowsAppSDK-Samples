// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "SystemFrame.h"
#include "TextVisual.h"

class RootFrame final : public SystemFrame, public IFrameHost
{
public:
    RootFrame(
        const winrt::Compositor& compositor,
        const winrt::WUC::Compositor& systemCompositor);

    LRESULT HandleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        _Out_ bool* handled) override;

    void ConnectLeftFrame(IFrame* frame);
    void ConnectRightFrame(IFrame* frame);

    // IFrameHost overrides.
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetParentProviderForChildFrame(_In_ IFrame const* const sender) const override;
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const sender) const override;

private:
    static constexpr float k_inset = 10.0f;

    static constexpr float k_minScale = 0.5f;
    static constexpr float k_maxScale = 2.0f;
    static constexpr float k_scaleIncrement = 0.25f;

    static constexpr float k_minRotation = 0.0f;
    static constexpr float k_maxRotation = 360.0f;
    static constexpr float k_rotationIncrement = 90.0f;

    winrt::Point PointFromLParam(
        LPARAM lParam) const;

    void InitializeVisualTree(
        const winrt::WUC::Compositor& compositor);
    void InitializeDocumentContent();
    void InitializeRibbonContent();

    void OnClick(
        const winrt::Point& point,
        bool isRightClick);

    void OnKeyPress(
        WPARAM wParam);

    void HandleContentLayout() override;
    void HandleDisplayScaleChanged();

    std::shared_ptr<VisualTreeNode> m_clickSquareRoot = nullptr;

    IFrame* m_leftFrame = nullptr;
    IFrame* m_rightFrame = nullptr;

    winrt::ChildContentLink m_leftChildLink = nullptr;
    winrt::ChildContentLink m_rightChildLink = nullptr;

    static constexpr wchar_t k_rootFrameName[] = L"Root";
    static constexpr wchar_t k_ribbonFrameName[] = L"Ribbon";
    static constexpr wchar_t k_documentFrameName[] = L"Document";

    SystemTextVisual m_rootLabel;
    SystemTextVisual m_ribbonLabel;

    std::shared_ptr<AutomationPeer> m_leftContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_rightContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_documentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_ribbonPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_ribbonContentPeer = nullptr;

    ContentIslandEnvironmentStateChanged_revoker m_islandEnvironmentStateChangedRevoker{};
};

