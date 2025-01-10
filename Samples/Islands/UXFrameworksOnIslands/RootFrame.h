// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "SystemFrame.h"
#include "TextVisual.h"
#include "CheckBox.h"

class RootFrame final : public SystemFrame, public IFrameHost
{
public:
    RootFrame(
        const winrt::DispatcherQueue& queue,
        const winrt::WUC::Compositor& systemCompositor,
        const std::shared_ptr<SettingCollection>& settings);

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
    static constexpr float k_ribbonHeight = 100.0f;

    static constexpr float k_minZoom = 0.5f;
    static constexpr float k_maxZoom = 2.0f;
    static constexpr float k_zoomIncrement = 0.25f;
    float m_zoomFactor = 1.0f;

    static constexpr float m_rotationAngleUnit = 3.14159f / 2;
    static constexpr int m_rotationAngleLimit = 4;
    int m_rotationAngle = 0;

    winrt::Point PointFromLParam(
        LPARAM lParam) const;

    void InitializeVisualTree(
        const winrt::WUC::Compositor& compositor);
    void InitializeDocumentContent();
    void InitializeRibbonContent();

    bool HitTestCheckBox(const winrt::Point& point, SystemCheckBox& control);

    void OnClick(
        const winrt::Point& point,
        bool isRightClick);

    void OnKeyPress(
        WPARAM wParam);

    void SetZoomFactor(float newZoom);

    void HandleContentLayout() override;
    void HandleDisplayScaleChanged();

    std::shared_ptr<VisualTreeNode> m_clickSquareRoot = nullptr;

    IFrame* m_leftFrame = nullptr;
    IFrame* m_rightFrame = nullptr;

    winrt::ChildSiteLink m_leftChildSiteLink = nullptr;
    winrt::ChildSiteLink m_rightChildSiteLink = nullptr;

    static constexpr wchar_t k_rootFrameName[] = L"Root";
    static constexpr wchar_t k_ribbonFrameName[] = L"Ribbon";
    static constexpr wchar_t k_documentFrameName[] = L"Document";

    SystemTextVisual m_rootLabel;
    SystemTextVisual m_ribbonLabel;
    SystemTextVisual m_backLabel;
    SystemTextVisual m_frontLabel;
    SystemCheckBox m_forceAliasedTextCheckBox;
    SystemCheckBox m_disablePixelSnappingCheckBox;
    SystemCheckBox m_showSpriteBoundsCheckBox;
    SystemCheckBox m_showSpriteGenerationCheckBox;

    winrt::WUC::ContainerVisual m_ribbonRootVisual{nullptr};
    winrt::WUC::ContainerVisual m_documentRootVisual{nullptr};
    winrt::WUC::ContainerVisual m_leftContentVisual{nullptr};
    winrt::WUC::ContainerVisual m_rightContentVisual{nullptr};
    winrt::WUC::SpriteVisual m_ribbonContentVisual{nullptr};
    winrt::WUC::ContainerVisual m_backContentVisual{nullptr};
    winrt::WUC::ContainerVisual m_frontContentVisual{nullptr};

    std::shared_ptr<AutomationPeer> m_leftContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_rightContentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_documentPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_ribbonPeer = nullptr;
    std::shared_ptr<AutomationPeer> m_ribbonContentPeer = nullptr;

    ContentIslandEnvironmentStateChanged_revoker m_islandEnvironmentStateChangedRevoker{};
};

