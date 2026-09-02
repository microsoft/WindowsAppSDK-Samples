// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "AutomationFragmentRoot.h"

namespace AutomationHelpers
{

std::unique_ptr<AutomationCallbackRevoker> AutomationFragmentRoot::SetFragmentRootCallbackHandler(
    IAutomationFragmentRootCallbackHandler* const handler)
{
    AddHandler(AutomationCallbackHandlerType::FragmentRoot, handler);
    return AutomationCallbackRevoker::Create(GetWeak(), handler);
}

HRESULT __stdcall AutomationFragmentRoot::ElementProviderFromPoint(
    _In_ double x,
    _In_ double y,
    _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragment)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != fragment)
        {
            *fragment = nullptr;
            if (auto handler = GetHandler<IAutomationFragmentRootCallbackHandler>(AutomationCallbackHandlerType::FragmentRoot))
            {
                handler->GetFragmentFromPointInScreenCoordinatesForAutomation(x, y, GetIUnknown<AutomationFragmentRoot>()).copy_to(fragment);
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

HRESULT __stdcall AutomationFragmentRoot::GetFocus(
    _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragmentInFocus)
{
    std::unique_lock lock{ m_mutex };
    try
    {
        if (nullptr != fragmentInFocus)
        {
            *fragmentInFocus = nullptr;
            if (auto handler = GetHandler<IAutomationFragmentRootCallbackHandler>(AutomationCallbackHandlerType::FragmentRoot))
            {
                handler->GetFragmentInFocusForAutomation(GetIUnknown<AutomationFragmentRoot>()).copy_to(fragmentInFocus);
            }
        }
    }
    catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
    return S_OK;
}

}
