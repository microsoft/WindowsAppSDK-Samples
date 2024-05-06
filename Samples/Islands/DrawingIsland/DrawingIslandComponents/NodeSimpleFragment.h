// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <mutex>

namespace winrt::DrawingIslandComponents
{
    struct __declspec(novtable) IAutomationCallbackHandler
    {
        virtual ~IAutomationCallbackHandler() noexcept = default;

        virtual winrt::Windows::Graphics::RectInt32 GetScreenBoundsForAutomationFragment(
            _In_ ::IUnknown const* const sender) const = 0;

        virtual winrt::com_ptr<IRawElementProviderFragment> GetFragmentFromPoint(
            _In_ double x,
            _In_ double y) const = 0;

        virtual winrt::com_ptr<IRawElementProviderFragment> GetFragmentInFocus() const = 0;
    };

    struct NodeSimpleFragment : winrt::implements<NodeSimpleFragment,
        IInspectable,
        IRawElementProviderSimple,
        IRawElementProviderFragment>

    {
        void AddChildToEnd(
            _In_ winrt::com_ptr<NodeSimpleFragment> const& child);

        void RemoveChild(
            _In_ winrt::com_ptr<NodeSimpleFragment> const& child);

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
            _Out_ ProviderOptions* providerOptions) final override;

        HRESULT __stdcall GetPatternProvider(
            _In_ PATTERNID patternId,
            _COM_Outptr_opt_result_maybenull_ IUnknown** patternProvider) final override;

        HRESULT __stdcall GetPropertyValue(
            _In_ PROPERTYID propertyId,
            _Out_ VARIANT* propertyValue) final override;

        HRESULT __stdcall get_HostRawElementProvider(
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderSimple** hostRawElementProviderSimple) final override;

        // IRawElementProviderFragment overrides.
        HRESULT __stdcall Navigate(
            _In_ NavigateDirection direction,
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragment) final override;

        HRESULT __stdcall GetRuntimeId(
            _Outptr_opt_result_maybenull_ SAFEARRAY** runtimeId) final override;

        HRESULT __stdcall get_BoundingRectangle(
            _Out_ UiaRect* boundingRectangle) final override;

        HRESULT __stdcall GetEmbeddedFragmentRoots(
            _Outptr_opt_result_maybenull_ SAFEARRAY** embeddedFragmentRoots) final override;

        HRESULT __stdcall SetFocus() final override;

        HRESULT __stdcall get_FragmentRoot(
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragmentRoot** fragmentRoot) final override;

    protected:
        mutable std::mutex m_mutex{};

        // We do not hold a strong reference to the object that owns us.
        // However we do need to be able to call back into it to get information.
        // The owner is responsible for unsetting itself when it gets destroyed.
        IAutomationCallbackHandler const* m_ownerNoRef{ nullptr };

    private:
        void SetParent(
            _In_ winrt::weak_ref<NodeSimpleFragment> const& parent);

        winrt::com_ptr<NodeSimpleFragment> GetParent() const;

        void SetPreviousSibling(
            _In_ winrt::weak_ref<NodeSimpleFragment> const& previousSibling);

        winrt::com_ptr<NodeSimpleFragment> GetPreviousSibling() const;

        void SetNextSibling(
            _In_ winrt::weak_ref<NodeSimpleFragment> const& nextSibling);

        winrt::com_ptr<NodeSimpleFragment> GetNextSibling() const;

        // Automatically generate unique runtime IDs per fragment.
        inline static unsigned __int32 s_nextAvailableInternalRuntimeId{ 0 };
        unsigned __int32 m_internalRuntimeId{ ++s_nextAvailableInternalRuntimeId };

        ProviderOptions m_providerOptions{ ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading };
        std::wstring m_name{ L"" };
        bool m_isContent{ true };
        bool m_isControl{ true };
        long m_uiaControlTypeId{ UIA_CustomControlTypeId };
        winrt::com_ptr<IRawElementProviderSimple> m_hostProvider{ nullptr };

        winrt::weak_ref<NodeSimpleFragment> m_parent{ nullptr };
        winrt::weak_ref<NodeSimpleFragment> m_previousSibling{ nullptr };
        winrt::weak_ref<NodeSimpleFragment> m_nextSibling{ nullptr };
        std::vector<winrt::com_ptr<NodeSimpleFragment>> m_children{};
    };
}
