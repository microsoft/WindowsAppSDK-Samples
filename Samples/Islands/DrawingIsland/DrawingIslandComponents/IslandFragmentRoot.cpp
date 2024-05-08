// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents
{
    HRESULT __stdcall IslandFragmentRoot::ElementProviderFromPoint(
        _In_ double x,
        _In_ double y,
        _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragment)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != fragment)
            {
                *fragment = nullptr;

                // This provider might still be alive in a UIA callback when the DrawingIsland is being torn down.
                // Make sure we still have a valid owner before proceeding to query the DrawingIsland for this information.
                if (nullptr != m_ownerNoRef)
                {
                    m_ownerNoRef->GetFragmentFromPoint(x, y).copy_to(fragment);
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall IslandFragmentRoot::GetFocus(
        _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragmentInFocus)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != fragmentInFocus)
            {
                *fragmentInFocus = nullptr;

                // This provider might still be alive in a UIA callback when the DrawingIsland is being torn down.
                // Make sure we still have a valid owner before proceeding to query the DrawingIsland for this information.
                if (nullptr != m_ownerNoRef)
                {
                    m_ownerNoRef->GetFragmentInFocus().copy_to(fragmentInFocus);
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }
}
