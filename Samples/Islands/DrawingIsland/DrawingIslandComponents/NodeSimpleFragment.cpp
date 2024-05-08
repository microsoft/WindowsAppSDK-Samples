// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "NodeSimpleFragment.h"

using unique_safearray = wil::unique_any<SAFEARRAY*, decltype(&::SafeArrayDestroy), ::SafeArrayDestroy>;

namespace winrt::DrawingIslandComponents
{
    void NodeSimpleFragment::AddChildToEnd(
        _In_ winrt::com_ptr<NodeSimpleFragment> const& child)
    {
        std::unique_lock lock{ m_mutex };

        if (nullptr == child)
        {
            // Nothing to do.
            return;
        }

        // The child should not already have a parent.
        winrt::check_bool(nullptr == child->GetParent());

        // Set us up as the parent for the new child.
        child->SetParent(get_weak());

        // Set up the sibling relationships.
        if (!m_children.empty())
        {
            auto& previousSiblingForNewChild = m_children.back();
            previousSiblingForNewChild->SetNextSibling(child);
            child->SetPreviousSibling(previousSiblingForNewChild);
        }

        // Finally add the child.
        m_children.push_back(child);
    }

    void NodeSimpleFragment::RemoveChild(
        _In_ winrt::com_ptr<NodeSimpleFragment> const& child)
    {
        std::unique_lock lock{ m_mutex };

        if (nullptr == child)
        {
            // Nothing to do.
            return;
        }

        auto iterator = std::find_if(
            m_children.begin(), m_children.end(), [&child](auto const& childEntry)
            {
                // See if we find a matching child entry in our children.
                return (childEntry.get() == child.get());
            });

        // We cannot remove a child that isn't ours.
        winrt::check_bool(m_children.end() != iterator);

        // Remove us from the parent relationship with the child.
        child->SetParent(nullptr);

        // Reset the sibling relationships.
        auto previousSibling = child->GetPreviousSibling();
        auto nextSibling = child->GetNextSibling();
        if (nullptr != previousSibling)
        {
            previousSibling->SetNextSibling(nextSibling);
        }
        if (nullptr != nextSibling)
        {
            nextSibling->SetPreviousSibling(previousSibling);
        }
        child->SetPreviousSibling(nullptr);
        child->SetNextSibling(nullptr);

        // Finally, remove the child.
        m_children.erase(iterator);
    }

    void NodeSimpleFragment::RemoveAllChildren()
    {
        std::unique_lock lock{ m_mutex };

        for (auto& child : m_children)
        {
            // Disconnect the relationships from all our children.
            child->SetParent(nullptr);
            child->SetPreviousSibling(nullptr);
            child->SetNextSibling(nullptr);
        }

        // Remove all the children.
        m_children.clear();
    }

    void NodeSimpleFragment::SetCallbackHandler(
        _In_opt_ IAutomationCallbackHandler const* const owner)
    {
        std::unique_lock lock{ m_mutex };
        m_ownerNoRef = owner;
    }

    void NodeSimpleFragment::SetProviderOptions(
        _In_ ProviderOptions const& providerOptions)
    {
        std::unique_lock lock{ m_mutex };
        m_providerOptions = providerOptions;
    }

    void NodeSimpleFragment::SetName(
        _In_ std::wstring_view const& name)
    {
        std::unique_lock lock{ m_mutex };
        m_name = name;
    }

    void NodeSimpleFragment::SetIsContent(
        _In_ bool const& isContent)
    {
        std::unique_lock lock{ m_mutex };
        m_isContent = isContent;
    }

    void NodeSimpleFragment::SetIsControl(
        _In_ bool const& isControl)
    {
        std::unique_lock lock{ m_mutex };
        m_isControl = isControl;
    }

    void NodeSimpleFragment::SetUiaControlTypeId(
        _In_ long const& uiaControlTypeId)
    {
        std::unique_lock lock{ m_mutex };
        m_uiaControlTypeId = uiaControlTypeId;
    }

    void NodeSimpleFragment::SetHostProvider(
        _In_ winrt::com_ptr<IRawElementProviderSimple> const& hostProvider)
    {
        std::unique_lock lock{ m_mutex };
        m_hostProvider = hostProvider;
    }

    HRESULT __stdcall NodeSimpleFragment::get_ProviderOptions(
        _Out_ ProviderOptions* providerOptions)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != providerOptions)
            {
                *providerOptions = m_providerOptions;
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::GetPatternProvider(
        _In_ PATTERNID patternId,
        _COM_Outptr_opt_result_maybenull_ IUnknown** patternProvider)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != patternProvider)
            {
                *patternProvider = nullptr;
                switch (patternId)
                {
                case UIA_InvokePatternId:
                {
                    if (auto invokeProvider = get_strong().try_as<IInvokeProvider>())
                    {
                        invokeProvider.as<IUnknown>().copy_to(patternProvider);
                    }
                    break;
                }
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::GetPropertyValue(
        _In_ PROPERTYID propertyId,
        _Out_ VARIANT* propertyValue)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
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

    HRESULT __stdcall NodeSimpleFragment::get_HostRawElementProvider(
        _COM_Outptr_opt_result_maybenull_ IRawElementProviderSimple** hostRawElementProviderSimple)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != hostRawElementProviderSimple)
            {
                m_hostProvider.copy_to(hostRawElementProviderSimple);
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::Navigate(
        _In_ NavigateDirection direction,
        _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragment** fragment)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != fragment)
            {
                *fragment = nullptr;
                switch (direction)
                {
                case NavigateDirection_Parent:
                {
                    if (auto strongParent = m_parent.get())
                    {
                        strongParent.as<IRawElementProviderFragment>().copy_to(fragment);
                    }
                    break;
                }
                case NavigateDirection_NextSibling:
                {
                    if (auto strongSibling = m_nextSibling.get())
                    {
                        strongSibling.as<IRawElementProviderFragment>().copy_to(fragment);
                    }
                    break;
                }
                case NavigateDirection_PreviousSibling:
                {
                    if (auto strongSibling = m_previousSibling.get())
                    {
                        strongSibling.as<IRawElementProviderFragment>().copy_to(fragment);
                    }
                    break;
                }
                case NavigateDirection_FirstChild:
                {
                    if (!m_children.empty())
                    {
                        m_children.front().as<IRawElementProviderFragment>().copy_to(fragment);
                    }
                    break;
                }
                case NavigateDirection_LastChild:
                {
                    if (!m_children.empty())
                    {
                        m_children.back().as<IRawElementProviderFragment>().copy_to(fragment);
                    }
                    break;
                }
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::GetRuntimeId(
        _Outptr_opt_result_maybenull_ SAFEARRAY** runtimeId)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != runtimeId)
            {
                *runtimeId = nullptr;

                std::array<unsigned __int32, 2> id{ UiaAppendRuntimeId, m_internalRuntimeId };
                unsigned long arraySizeAsUnsignedLong = static_cast<unsigned long>(id.size());

                unique_safearray runtimeIdArray{ ::SafeArrayCreateVector(VT_I4, 0, arraySizeAsUnsignedLong) };
                SAFEARRAY* rawPointerToSafeArray = runtimeIdArray.get();
                winrt::check_pointer(rawPointerToSafeArray);

                for (long i = 0; i < static_cast<long>(arraySizeAsUnsignedLong); ++i)
                {
                    winrt::check_hresult(::SafeArrayPutElement(rawPointerToSafeArray, &i, &(id[i])));
                }

                *runtimeId = runtimeIdArray.release();
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::get_BoundingRectangle(
        _Out_ UiaRect* boundingRectangle)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != boundingRectangle)
            {
                *boundingRectangle = { 0, 0, 0, 0 };

                // This provider might still be alive in a UIA callback when the DrawingIsland is being torn down.
                // Make sure we still have a valid owner before proceeding to query the DrawingIsland for this information.
                if (nullptr != m_ownerNoRef)
                {
                    auto screenRectangle =
                        m_ownerNoRef->GetScreenBoundsForAutomationFragment(get_strong().as<::IUnknown>().get());

                    boundingRectangle->left = screenRectangle.X;
                    boundingRectangle->top = screenRectangle.Y;
                    boundingRectangle->width = screenRectangle.Width;
                    boundingRectangle->height = screenRectangle.Height;
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::GetEmbeddedFragmentRoots(
        _Outptr_opt_result_maybenull_ SAFEARRAY** embeddedFragmentRoots)
    {
        if (nullptr != embeddedFragmentRoots)
        {
            *embeddedFragmentRoots = nullptr;
        }
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::SetFocus()
    {
        return S_OK;
    }

    HRESULT __stdcall NodeSimpleFragment::get_FragmentRoot(
        _COM_Outptr_opt_result_maybenull_ IRawElementProviderFragmentRoot** fragmentRoot)
    {
        try
        {
            std::unique_lock lock{ m_mutex };
            if (nullptr != fragmentRoot)
            {
                *fragmentRoot = nullptr;

                // Walk up our fragment tree until we find our fragment root.
                auto fragmentRootCandidate = get_strong();
                bool currentCandidateIsThisObject = true;
                while (nullptr != fragmentRootCandidate && nullptr == fragmentRootCandidate.try_as<IRawElementProviderFragmentRoot>())
                {
                    // Haven't found the fragment root yet, keep walking up our tree.
                    fragmentRootCandidate = currentCandidateIsThisObject ?
                        m_parent.get() : fragmentRootCandidate->GetParent();

                    // Once we start walking up the tree, we must ensure we're thread-safe and call through GetParent on the other objects.
                    currentCandidateIsThisObject = false;
                }

                if (nullptr != fragmentRootCandidate)
                {
                    // Found the fragment root, return it.
                    fragmentRootCandidate.as<IRawElementProviderFragmentRoot>().copy_to(fragmentRoot);
                }
            }
        }
        catch (...) { return UIA_E_ELEMENTNOTAVAILABLE; }
        return S_OK;
    }

    void NodeSimpleFragment::SetParent(
        _In_ winrt::weak_ref<NodeSimpleFragment> const& parent)
    {
        std::unique_lock lock{ m_mutex };
        m_parent = parent;
    }

    winrt::com_ptr<NodeSimpleFragment> NodeSimpleFragment::GetParent() const
    {
        std::unique_lock lock{ m_mutex };
        return m_parent.get();
    }

    void NodeSimpleFragment::SetPreviousSibling(
        _In_ winrt::weak_ref<NodeSimpleFragment> const& previousSibling)
    {
        std::unique_lock lock{ m_mutex };
        m_previousSibling = previousSibling;
    }

    winrt::com_ptr<NodeSimpleFragment> NodeSimpleFragment::GetPreviousSibling() const
    {
        std::unique_lock lock{ m_mutex };
        return m_previousSibling.get();
    }

    void NodeSimpleFragment::SetNextSibling(
        _In_ winrt::weak_ref<NodeSimpleFragment> const& nextSibling)
    {
        std::unique_lock lock{ m_mutex };
        m_nextSibling = nextSibling;
    }

    winrt::com_ptr<NodeSimpleFragment> NodeSimpleFragment::GetNextSibling() const
    {
        std::unique_lock lock{ m_mutex };
        return m_nextSibling.get();
    }
}
