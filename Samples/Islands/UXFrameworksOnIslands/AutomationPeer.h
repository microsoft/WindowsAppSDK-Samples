// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <wil/cppwinrt.h>
#include "VisualTreeNode.h"
#include "AutomationFragment.h"
#include "IFrame.h"
#include "EventRevokers.h"

struct AutomationPeer : std::enable_shared_from_this<AutomationPeer>,
    AutomationHelpers::IAutomationFragmentCallbackHandler,
    AutomationHelpers::IAutomationExternalChildCallbackHandler
{
    [[nodiscard]] static std::shared_ptr<AutomationPeer> Create(
        _In_ IFrame const* const owningFrame,
        _In_ std::shared_ptr<VisualTreeNode> const& visual,
        _In_ std::wstring_view const& name,
        _In_ long const& uiaControlTypeId) noexcept;

    explicit AutomationPeer() noexcept = default;
    ~AutomationPeer() noexcept = default;

    // Disable move and copy.
    explicit AutomationPeer(AutomationPeer const&) = delete;
    explicit AutomationPeer(AutomationPeer&&) = delete;
    AutomationPeer& operator=(AutomationPeer const&) = delete;
    AutomationPeer& operator=(AutomationPeer&&) = delete;

    // AutomationHelpers::IAutomationFragmentCallbackHandler overrides.
    winrt::Windows::Graphics::RectInt32 GetBoundingRectangleInScreenCoordinatesForAutomation(
        ::IUnknown const* const sender) const final override;
    void HandleSetFocusForAutomation(
        ::IUnknown const* const sender) final override;

    // AutomationHelpers::IAutomationExternalChildCallbackHandler overrides.
    winrt::com_ptr<::IRawElementProviderFragment> GetFirstChildFragmentForAutomation(
        ::IUnknown const* const sender) const final override;
    winrt::com_ptr<::IRawElementProviderFragment> GetLastChildFragmentForAutomation(
        ::IUnknown const* const sender) const final override;

    winrt::com_ptr<AutomationHelpers::AutomationFragment> Fragment() const noexcept { std::unique_lock lock{ m_mutex }; return m_fragment; }
    std::shared_ptr<VisualTreeNode> VisualNode() const noexcept { std::unique_lock lock{ m_mutex }; return m_visual; }

    void SetChildSiteLink(_In_ winrt::Microsoft::UI::Content::ChildSiteLink const& childSiteLink);
    void SetChildFrame(_In_ IFrame const* const childFrame);
    [[nodiscard]] winrt::com_ptr<::IRawElementProviderFragment> ForwardFragmentFromPointInScreenCoordinatesRequest(double x, double y) const;

private:
    void Initialize(
        _In_ IFrame const* const owningFrame,
        _In_ std::shared_ptr<VisualTreeNode> const& visual,
        _In_ std::wstring_view const& name,
        _In_ long const& uiaControlTypeId);

    mutable std::mutex m_mutex{};

    IFrame const* m_owningFrameNoRef{ nullptr };
    IFrame const* m_childFrameNoRef{ nullptr };
    winrt::Microsoft::UI::Content::ChildSiteLink m_childSiteLink{ nullptr };

    std::shared_ptr<VisualTreeNode> m_visual{ nullptr };
    winrt::com_ptr<AutomationHelpers::AutomationFragment> m_fragment{ nullptr };
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_fragmentCallbackRevoker{ nullptr };
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_externalChildCallbackRevoker{ nullptr };

    ChildSiteLinkParentAutomationProviderRequested_revoker m_parentAutomationProviderRequestedRevoker{};
    ChildSiteLinkPreviousSiblingAutomationProviderRequested_revoker m_previousSiblingAutomationProviderRequestedRevoker{};
    ChildSiteLinkNextSiblingAutomationProviderRequested_revoker m_nextSiblingAutomationProviderRequestedRevoker{};
    ChildSiteLinkFragmentRootAutomationProviderRequested_revoker m_fragmentRootAutomationProviderRequestedRevoker{};
};
