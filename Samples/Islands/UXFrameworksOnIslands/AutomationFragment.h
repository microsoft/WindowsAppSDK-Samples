// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "AutomationElement.h"
#include "AutomationCallbackRevoker.h"

namespace AutomationHelpers
{

struct AutomationFragment : winrt::implements<AutomationFragment, AutomationElement, ::IRawElementProviderFragment>
{
    // Automation callback handlers.
    [[nodiscard]] std::unique_ptr<AutomationCallbackRevoker> SetFragmentCallbackHandler(
        IAutomationFragmentCallbackHandler* const handler);

    [[nodiscard]] std::unique_ptr<AutomationCallbackRevoker> SetExternalChildCallbackHandler(
        IAutomationExternalChildCallbackHandler* const handler);

    [[nodiscard]] std::unique_ptr<AutomationCallbackRevoker> SetExternalParentCallbackHandler(
        IAutomationExternalParentCallbackHandler* const handler);

    // Methods.
    void AddChildToEnd(
        winrt::com_ptr<AutomationFragment> const& child);

    void RemoveChild(
        winrt::com_ptr<AutomationFragment> const& child);

    void RemoveAllChildren();

    // IRawElementProviderFragment implementation.
    HRESULT __stdcall Navigate(
        _In_ NavigateDirection direction,
        _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragment) final override;

    HRESULT __stdcall GetRuntimeId(
        _Outptr_opt_result_maybenull_ SAFEARRAY** runtimeId) final override;

    HRESULT __stdcall get_BoundingRectangle(
        _Out_ UiaRect* boundingRectangle) final override;

    HRESULT __stdcall GetEmbeddedFragmentRoots(
        _Outptr_opt_result_maybenull_ SAFEARRAY** embeddedFragmentRoots) final override;

    HRESULT __stdcall SetFocus() final override;

    HRESULT __stdcall get_FragmentRoot(
        _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragmentRoot** fragmentRoot) final override;

private:
    // Property setters. Note these private methods hold the lock since they are called from the parent AutomationFragment on a child AutomationFragment.
    void Parent(winrt::weak_ref<AutomationBase> const& parent) { std::unique_lock lock{ m_mutex }; m_parent = parent; }
    void PreviousSibling(winrt::weak_ref<AutomationBase> const& previousSibling) { std::unique_lock lock{ m_mutex }; m_previousSibling = previousSibling; }
    void NextSibling(winrt::weak_ref<AutomationBase> const& nextSibling) { std::unique_lock lock{ m_mutex }; m_nextSibling = nextSibling; }

    // Property getters. Note these private methods hold the lock since they are called from the parent AutomationFragment on a child AutomationFragment.
    winrt::com_ptr<AutomationFragment> Parent() const { std::unique_lock lock{ m_mutex }; return LockWeak<AutomationFragment>(m_parent); }
    winrt::com_ptr<AutomationFragment> PreviousSibling() const { std::unique_lock lock{ m_mutex }; return LockWeak<AutomationFragment>(m_previousSibling); }
    winrt::com_ptr<AutomationFragment> NextSibling() const { std::unique_lock lock{ m_mutex }; return LockWeak<AutomationFragment>(m_nextSibling); }
    int* RuntimeId() { std::unique_lock lock{ m_mutex }; return reinterpret_cast<int*>(&(m_runtimeId[0])); }
    int RuntimeIdSize() const { std::unique_lock lock{ m_mutex }; return static_cast<int>(m_runtimeId.size()); }

    // Guard via m_mutex when accessing the following data members from all methods
    // that can be called from outside the AutomationHelpers namespace.
    // Automatically generate unique runtime IDs per fragment.
    inline static unsigned __int32 s_nextAvailableInternalRuntimeId{ 0 };
    std::array<unsigned __int32, 2> m_runtimeId{ UiaAppendRuntimeId, ++s_nextAvailableInternalRuntimeId };

    winrt::weak_ref<AutomationBase> m_parent{ nullptr };
    winrt::weak_ref<AutomationBase> m_previousSibling{ nullptr };
    winrt::weak_ref<AutomationBase> m_nextSibling{ nullptr };
    std::vector<winrt::com_ptr<AutomationFragment>> m_children{};
    std::vector<winrt::com_ptr<AutomationFragment>> m_embeddedFragments{};
};

}
