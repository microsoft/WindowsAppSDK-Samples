// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <mutex>
#include "AutomationCallbackHandler.h"

namespace winrt::DrawingIslandComponents
{
    struct AutomationFragment : winrt::implements<AutomationFragment,
        IInspectable,
        IRawElementProviderSimple,
        IRawElementProviderFragment>
    {
        void AddChildToEnd(
            _In_ winrt::com_ptr<AutomationFragment> const& child);

        void RemoveChild(
            _In_ winrt::com_ptr<AutomationFragment> const& child);

        void RemoveAllChildren();

        void SetCallbackHandler(
            _In_opt_ IAutomationCallbackHandler const* const owner);

        void SetProviderOptions(
            _In_ ProviderOptions const& providerOptions);

        void SetName(
            _In_ std::wstring_view const& name);

        void SetIsContent(
            _In_ bool const& isContent);

        void SetIsControl(
            _In_ bool const& isControl);

        void SetUiaControlTypeId(
            _In_ long const& uiaControlTypeId);

        void SetHostProvider(
            _In_ winrt::com_ptr<IRawElementProviderSimple> const& hostProvider);

        // IRawElementProviderSimple overrides.
        HRESULT __stdcall get_ProviderOptions(
            _Out_ ProviderOptions* providerOptions
        ) final override;

        HRESULT __stdcall GetPatternProvider(
            _In_ PATTERNID patternId,
            _COM_Outptr_opt_result_maybenull_ IUnknown** patternProvider
        ) final override;

        HRESULT __stdcall GetPropertyValue(
            _In_ PROPERTYID propertyId,
            _Out_ VARIANT* propertyValue
        ) final override;

        HRESULT __stdcall get_HostRawElementProvider(
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderSimple** hostRawElementProviderSimple
        ) final override;

        // IRawElementProviderFragment overrides.
        HRESULT __stdcall Navigate(
            _In_ NavigateDirection direction,
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragment
        ) final override;

        HRESULT __stdcall GetRuntimeId(
            _Outptr_opt_result_maybenull_ SAFEARRAY** runtimeId
        ) final override;

        HRESULT __stdcall get_BoundingRectangle(
            _Out_ UiaRect* boundingRectangle
        ) final override;

        HRESULT __stdcall GetEmbeddedFragmentRoots(
            _Outptr_opt_result_maybenull_ SAFEARRAY** embeddedFragmentRoots
        ) final override;

        HRESULT __stdcall SetFocus() final override;

        HRESULT __stdcall get_FragmentRoot(
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragmentRoot** fragmentRoot
        ) final override;

    protected:
        mutable std::mutex m_mutex{};

        // We do not hold a strong reference to the object that owns us.
        // However we do need to be able to call back into it to get information.
        // The owner is responsible for unsetting itself when it gets destroyed.
        IAutomationCallbackHandler const* m_ownerNoRef{ nullptr };

    private:
        void SetParent(
            _In_ winrt::weak_ref<AutomationFragment> const& parent);

        winrt::com_ptr<AutomationFragment> GetParent() const;

        void SetPreviousSibling(
            _In_ winrt::weak_ref<AutomationFragment> const& previousSibling);

        winrt::com_ptr<AutomationFragment> GetPreviousSibling() const;

        void SetNextSibling(
            _In_ winrt::weak_ref<AutomationFragment> const& nextSibling);

        winrt::com_ptr<AutomationFragment> GetNextSibling() const;

        // Automatically generate unique runtime IDs per fragment.
        inline static unsigned __int32 s_nextAvailableInternalRuntimeId{ 0 };
        std::array<unsigned __int32, 2> m_runtimeId
            { UiaAppendRuntimeId, ++s_nextAvailableInternalRuntimeId };

        ProviderOptions m_providerOptions
            { ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading };
        std::wstring m_name{ L"" };
        bool m_isContent{ true };
        bool m_isControl{ true };
        long m_uiaControlTypeId{ UIA_CustomControlTypeId };
        winrt::com_ptr<IRawElementProviderSimple> m_hostProvider{ nullptr };

        winrt::weak_ref<AutomationFragment> m_parent{ nullptr };
        winrt::weak_ref<AutomationFragment> m_previousSibling{ nullptr };
        winrt::weak_ref<AutomationFragment> m_nextSibling{ nullptr };
        std::vector<winrt::com_ptr<AutomationFragment>> m_children{};
    };
}
