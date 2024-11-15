// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "AutomationFragment.h"

namespace winrt::DrawingIslandComponents
{
    struct AutomationFragmentRoot : winrt::implements<AutomationFragmentRoot,
        AutomationFragment,
        IRawElementProviderFragmentRoot>
    {
        // IRawElementProviderFragmentRoot overrides.
        HRESULT __stdcall ElementProviderFromPoint(
            _In_ double x,
            _In_ double y,
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragment
        ) final override;

        HRESULT __stdcall GetFocus(
            _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragmentInFocus
        ) final override;
    };
}
