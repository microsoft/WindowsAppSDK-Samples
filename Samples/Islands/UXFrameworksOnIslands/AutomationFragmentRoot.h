// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "AutomationFragment.h"

namespace AutomationHelpers
{

struct AutomationFragmentRoot : winrt::implements<AutomationFragmentRoot, AutomationFragment, ::IRawElementProviderFragmentRoot>
{
    // Automation callback handler.
    [[nodiscard]] std::unique_ptr<AutomationCallbackRevoker> SetFragmentRootCallbackHandler(
        IAutomationFragmentRootCallbackHandler* const handler);

    // IRawElementProviderFragmentRoot implementation.
    HRESULT __stdcall ElementProviderFromPoint(
        _In_ double x,
        _In_ double y,
        _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragment) final override;

    HRESULT __stdcall GetFocus(
        _COM_Outptr_opt_result_maybenull_ ::IRawElementProviderFragment** fragmentInFocus) final override;
};

}
