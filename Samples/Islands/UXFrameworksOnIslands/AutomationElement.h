// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "AutomationBase.h"

namespace AutomationHelpers
{

struct AutomationElement : winrt::implements<AutomationElement, AutomationBase, ::IRawElementProviderSimple>
{
    // Settable properties.
    void AutomationProviderOptions(::ProviderOptions const& providerOptions) { std::unique_lock lock{ m_mutex }; m_providerOptions = providerOptions; }
    void Name(std::wstring_view const& name) { std::unique_lock lock{ m_mutex }; m_name = name; }
    void IsContent(bool const& isContent) { std::unique_lock lock{ m_mutex }; m_isContent = isContent; }
    void IsControl(bool const& isControl) { std::unique_lock lock{ m_mutex }; m_isControl = isControl; }
    void UiaControlTypeId(long const& uiaControlTypeId) { std::unique_lock lock{ m_mutex }; m_uiaControlTypeId = uiaControlTypeId; }
    void HostProvider(winrt::com_ptr<::IRawElementProviderSimple> const& hostProvider) { std::unique_lock lock{ m_mutex }; m_hostProvider = hostProvider; }
    void HostProviderFromHwnd(HWND const& hwnd) { std::unique_lock lock{ m_mutex }; winrt::check_hresult(::UiaHostProviderFromHwnd(hwnd, m_hostProvider.put())); }

    // IRawElementProviderSimple implementation.
    HRESULT __stdcall get_ProviderOptions(
        _Out_ ::ProviderOptions* providerOptions) final override;

    HRESULT __stdcall GetPatternProvider(
        _In_ PATTERNID patternId,
        _COM_Outptr_opt_result_maybenull_ ::IUnknown** patternProvider) final override;

    HRESULT __stdcall GetPropertyValue(
        _In_ PROPERTYID propertyId,
        _Out_ VARIANT* propertyValue) final override;

    HRESULT __stdcall get_HostRawElementProvider(
        _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderSimple** hostRawElementProviderSimple) final override;

private:
    // Guard via m_mutex when accessing the following data members from all methods
    // that can be called from outside the AutomationHelpers namespace.
    ::ProviderOptions m_providerOptions{ ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading };
    std::wstring m_name{ L"" };
    bool m_isContent{ true };
    bool m_isControl{ true };
    long m_uiaControlTypeId{ UIA_CustomControlTypeId };
    winrt::com_ptr<::IRawElementProviderSimple> m_hostProvider{ nullptr };
};

}
