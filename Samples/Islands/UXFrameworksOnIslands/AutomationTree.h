// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include <wil/cppwinrt.h>
#include "IFrameHost.h"
#include "AutomationFragmentRoot.h"
#include "AutomationPeer.h"
#include "EventRevokers.h"

struct AutomationTree : std::enable_shared_from_this<AutomationTree>,
    AutomationHelpers::IAutomationFragmentCallbackHandler,
    AutomationHelpers::IAutomationFragmentRootCallbackHandler,
    AutomationHelpers::IAutomationExternalParentCallbackHandler
{
    [[nodiscard]] static std::shared_ptr<AutomationTree> Create(
        _In_ IFrame const* const owningFrame,
        _In_ std::wstring_view const& name,
        _In_ long const& uiaControlTypeId) noexcept;

    explicit AutomationTree() noexcept = default;
    ~AutomationTree() noexcept = default;

    // Disable move and copy.
    explicit AutomationTree(AutomationTree const&) = delete;
    explicit AutomationTree(AutomationTree&&) = delete;
    AutomationTree& operator=(AutomationTree const&) = delete;
    AutomationTree& operator=(AutomationTree&&) = delete;

    // AutomationHelpers::IAutomationFragmentCallbackHandler overrides.
    winrt::Windows::Graphics::RectInt32 GetBoundingRectangleInScreenCoordinatesForAutomation(
        ::IUnknown const* const sender) const final override;
    void HandleSetFocusForAutomation(
        ::IUnknown const* const sender) final override;

    // AutomationHelpers::IAutomationFragmentRootCallbackHandler overrides.
    winrt::com_ptr<::IRawElementProviderFragment> GetFragmentFromPointInScreenCoordinatesForAutomation(
        double x,
        double y,
        ::IUnknown const* const sender) const final override;
    winrt::com_ptr<::IRawElementProviderFragment> GetFragmentInFocusForAutomation(
        ::IUnknown const* const sender) const final override;

    // AutomationHelpers::IAutomationExternalParentCallbackHandler overrides.
    winrt::com_ptr<::IRawElementProviderFragment> GetParentFragmentForAutomation(
        ::IUnknown const* const sender) const final override;
    winrt::com_ptr<::IRawElementProviderFragment> GetNextSiblingFragmentForAutomation(
        ::IUnknown const* const sender) const final override;
    winrt::com_ptr<::IRawElementProviderFragment> GetPreviousSiblingFragmentForAutomation(
        ::IUnknown const* const sender) const final override;
    winrt::com_ptr<::IRawElementProviderFragmentRoot> GetFragmentRootForAutomation(
        ::IUnknown const* const sender) const final override;

    [[nodiscard]] winrt::com_ptr<AutomationHelpers::AutomationFragmentRoot> Root() const noexcept { std::unique_lock lock{ m_mutex }; return m_root; }

    [[nodiscard]] std::shared_ptr<AutomationPeer> CreatePeer(
        _In_ std::shared_ptr<VisualTreeNode> const& visual,
        _In_ std::wstring_view const& name,
        _In_ long const& uiaControlTypeId);

    void AddPeer(_In_ std::shared_ptr<AutomationPeer> const& peer);

    void SetFrameHost(_In_ IFrameHost const* const frameHost) noexcept;

private:
    void Initialize(
        _In_ IFrame const* const owningFrame,
        _In_ std::wstring_view const& name,
        _In_ long const& uiaControlTypeId);

    mutable std::mutex m_mutex{};

    IFrame const* m_owningFrameNoRef{ nullptr };
    IFrameHost const* m_frameHostNoRef{ nullptr };

    winrt::com_ptr<AutomationHelpers::AutomationFragmentRoot> m_root{ nullptr };
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_fragmentCallbackRevoker{ nullptr };
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_fragmentRootCallbackRevoker{ nullptr };
    std::unique_ptr<AutomationHelpers::AutomationCallbackRevoker> m_externalParentCallbackRevoker{ nullptr };

    std::vector<std::shared_ptr<AutomationPeer>> m_peers;

    ContentIslandAutomationProviderRequested_revoker m_contentIslandAutomationProviderRequestedRevoker{};
};
