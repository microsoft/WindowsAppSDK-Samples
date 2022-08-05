// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "IslandFragmentRoot.h"
#include "NodeSimpleFragment.h"

using unique_safearray = wil::unique_any<SAFEARRAY*, decltype(&::SafeArrayDestroy), ::SafeArrayDestroy>;

namespace Squares
{
    NodeSimpleFragment::NodeSimpleFragment(
        _In_z_ const wchar_t* name,
        int id) :
        m_name(name),
        m_id(id)
    {
    }

    void
    NodeSimpleFragment::AddChild(
        winrt::com_ptr<IslandFragmentRoot> child)
    {
        m_children.push_back(child);
    }

    std::vector<winrt::com_ptr<IslandFragmentRoot>>
    NodeSimpleFragment::GetChildren()
    {
        return m_children;
    }

    void
    NodeSimpleFragment::SetParent(
        winrt::com_ptr<IslandFragmentRoot> parent)
    {
        m_parent = (parent);
    }

    void
    NodeSimpleFragment::SetBoundingRects(
            UiaRect rect)
    {
        m_boundingRect = rect;
    }

    // IRawElementProviderSimple methods
    IFACEMETHODIMP
    NodeSimpleFragment::get_ProviderOptions(
        _Out_ ProviderOptions* retVal)
    {
        *retVal = (ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading);
        return S_OK;
    }

    IFACEMETHODIMP
    NodeSimpleFragment::GetPatternProvider(
        PATTERNID /*patternId*/,
        _Outptr_ IUnknown** retVal) try
    {
        *retVal = nullptr;
        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP
    NodeSimpleFragment::GetPropertyValue(
        PROPERTYID propertyId,
        _Out_ VARIANT* retVal) try
    {
        ::VariantInit(retVal);

        if (propertyId == UIA_NamePropertyId)
        {
            retVal->bstrVal = wil::make_bstr(m_name.c_str()).release();
            retVal->vt = VT_BSTR;
        }

        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP
    NodeSimpleFragment::get_HostRawElementProvider(
        _Outptr_ IRawElementProviderSimple** retVal)
    {
        *retVal = nullptr;
        return S_OK;
    }

    winrt::com_ptr<NodeSimpleFragment>
    NodeSimpleFragment::GetPreviousSibling()
    {
        winrt::com_ptr<NodeSimpleFragment> previous = nullptr;
        if (m_parent != nullptr)
        {
            std::vector<winrt::com_ptr<NodeSimpleFragment>> siblings = m_parent->GetChildren();
            int index = GetCurrentIndexFromSiblings(siblings);

            if (index != -1)
            {
                if (index > 0)
                {
                    previous = siblings[index - 1];
                }
            }
        }
        return previous;
    }

    winrt::com_ptr<NodeSimpleFragment>
    NodeSimpleFragment::GetNextSibling()
    {
        winrt::com_ptr<NodeSimpleFragment> next = nullptr;
        if (m_parent != nullptr)
        {
            std::vector<winrt::com_ptr<NodeSimpleFragment>> siblings = m_parent->GetChildren();
            int index = GetCurrentIndexFromSiblings(siblings);

            if (index != -1)
            {
                if ((index + 1) < static_cast<int>(siblings.size()))
                {
                    next = siblings[index + 1];
                }
            }
        }
        return next;
    }

    int
    NodeSimpleFragment::GetCurrentIndexFromSiblings(
        std::vector<winrt::com_ptr<NodeSimpleFragment>> siblings)
    {
        auto it = std::find_if(
                    siblings.begin(),
                    siblings.end(),
                    [this](const winrt::com_ptr<NodeSimpleFragment>& v) -> bool { return (v.get() == this); });

        int index = -1;
        if (it != siblings.end())
        {
            index = (int)(it - siblings.begin());
        }

        return index;
    }

    // IRawElementProviderFragment methods
    IFACEMETHODIMP
    NodeSimpleFragment::Navigate(
        _In_ NavigateDirection direction,
        _Outptr_ IRawElementProviderFragment** retVal) try
    {
        *retVal = nullptr;

        winrt::com_ptr<IRawElementProviderFragment> result;
        switch (direction)
        {
        case NavigateDirection_Parent:
            if (m_parent)
            {
                result = m_parent.as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_FirstChild:
            if (!m_children.empty())
            {
                result = m_children.front().as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_LastChild:
            if (!m_children.empty())
            {
                result = m_children.back().as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_NextSibling:
            {
                winrt::com_ptr<NodeSimpleFragment> nextSibling = GetNextSibling();
                if (nextSibling)
                {
                    result = nextSibling.as<IRawElementProviderFragment>();
                }
            }
            break;

        case NavigateDirection_PreviousSibling:
            {
                winrt::com_ptr<NodeSimpleFragment> previousSibling = GetPreviousSibling();
                if (previousSibling)
                {
                    result = previousSibling.as<IRawElementProviderFragment>();
                }
            }
            break;

        }

        *retVal = result.detach();
        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP
    NodeSimpleFragment::GetRuntimeId(
        _Outptr_ SAFEARRAY** retVal)
    {
        *retVal = nullptr;

        std::array<int, 2> runtimeId = { UiaAppendRuntimeId, m_id };

        unique_safearray runtimeIdArray{ ::SafeArrayCreateVector(
                VT_I4,
                0,
                static_cast<unsigned long>(runtimeId.size())) };
        THROW_HR_IF_NULL(E_FAIL, runtimeIdArray.get());
        for (long i = 0; i < static_cast<long>(runtimeId.size()); ++i)
        {
            ::SafeArrayPutElement(runtimeIdArray.get(), &i, &runtimeId[i]);
        }

        *retVal = runtimeIdArray.release();
        return S_OK;
    }

    IFACEMETHODIMP
    NodeSimpleFragment::get_BoundingRectangle(
        _Out_ UiaRect* retVal)
    {
        *retVal = m_boundingRect;
        return S_OK;
    }

    IFACEMETHODIMP
    NodeSimpleFragment::GetEmbeddedFragmentRoots(
        _Outptr_ SAFEARRAY** retVal)
    {
        *retVal = nullptr;
        return S_OK;
    }

    IFACEMETHODIMP
    NodeSimpleFragment::SetFocus()
    {
        return S_OK;
    }

    IFACEMETHODIMP
    NodeSimpleFragment::get_FragmentRoot(
        _Outptr_ IRawElementProviderFragmentRoot** retVal) try
    {
        if (m_parent)
        {
            winrt::com_ptr<IRawElementProviderFragmentRoot> result = m_parent.as<IRawElementProviderFragmentRoot>();
            *retVal = result.detach();
        }

        return S_OK;
    }
    CATCH_RETURN();

}
