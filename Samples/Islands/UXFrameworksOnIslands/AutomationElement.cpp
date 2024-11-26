// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationElement.h"

namespace AutomationHelpers
{

HRESULT __stdcall AutomationElement::get_ProviderOptions(
    _Out_ ::ProviderOptions* providerOptions)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != providerOptions)
        {
            *providerOptions = m_providerOptions;
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationElement::GetPatternProvider(
    _In_ PATTERNID patternId,
    _COM_Outptr_opt_result_maybenull_ ::IUnknown** patternProvider)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != patternProvider)
        {
            *patternProvider = nullptr;
            switch (patternId)
            {
                case UIA_InvokePatternId:
                {
                    if (auto invokeProvider = get_strong().try_as<::IInvokeProvider>())
                    {
                        invokeProvider.as<::IUnknown>().copy_to(patternProvider);
                    }
                    break;
                }
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationElement::GetPropertyValue(
    _In_ PROPERTYID propertyId,
    _Out_ VARIANT* propertyValue)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != propertyValue)
        {
            ::VariantInit(propertyValue);
            switch (propertyId)
            {
                case UIA_NamePropertyId:
                {
                    propertyValue->bstrVal = wil::make_bstr(m_name.c_str()).release();
                    propertyValue->vt = VT_BSTR;
                    break;
                }

                case UIA_IsContentElementPropertyId:
                {
                    propertyValue->boolVal = m_isContent ? VARIANT_TRUE : VARIANT_FALSE;
                    propertyValue->vt = VT_BOOL;
                    break;
                }

                case UIA_IsControlElementPropertyId:
                {
                    propertyValue->boolVal = m_isControl ? VARIANT_TRUE : VARIANT_FALSE;
                    propertyValue->vt = VT_BOOL;
                    break;
                }

                case UIA_ControlTypePropertyId:
                {
                    if (m_isControl)
                    {
                        propertyValue->vt = VT_I4;
                        propertyValue->lVal = m_uiaControlTypeId;
                    }
                    break;
                }
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationElement::get_HostRawElementProvider(
    _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderSimple** hostRawElementProviderSimple)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != hostRawElementProviderSimple)
        {
            m_hostProvider.copy_to(hostRawElementProviderSimple);
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

}
