// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "IslandFragmentRoot.h"
#include "NodeSimpleFragment.h"

using unique_safearray = wil::unique_any<SAFEARRAY*, decltype(&::SafeArrayDestroy), ::SafeArrayDestroy>;

DECLARE_INTERFACE_IID_(IContentIslandAccessibility, IUnknown, "F2DEC451-ABE3-4E84-AAE7-9EAD26F8BD5D")
{
    IFACEMETHOD(GetAutomationHostProvider)(
        _COM_Outptr_ IUnknown * *hostProvider
        ) PURE;
};

namespace Squares
{
    void
    IslandFragmentRoot::AddChild(
        winrt::com_ptr<NodeSimpleFragment> child)
    {
        m_children.push_back(child);
    }

    std::vector<winrt::com_ptr<NodeSimpleFragment>>
    IslandFragmentRoot::GetChildren()
    {
        return m_children;
    }

    void
    IslandFragmentRoot::SetParent(
        winrt::com_ptr<NodeSimpleFragment> parent)
    {
        m_parent = (parent);
    }

    void
    IslandFragmentRoot::SetName(
        std::wstring name)
    {
        m_name = std::move(name);
    }

    // IRawElementProviderSimple methods
    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::get_ProviderOptions(
        _Out_ ProviderOptions* retVal)
    {
        *retVal = (ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading | ProviderOptions_RefuseNonClientSupport);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::GetPatternProvider(
        PATTERNID /* idPattern */,
        _Outptr_result_maybenull_ IUnknown** retVal) try
    {
        *retVal = nullptr;
        return S_OK;
    }
    CATCH_RETURN();

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::GetPropertyValue(
        PROPERTYID idProp,
        _Out_ VARIANT* retVal) try
    {
        ::VariantInit(retVal);

        switch (idProp)
        {
        case UIA_NamePropertyId:
        {
            retVal->bstrVal = wil::make_bstr(m_name.c_str()).release();
            retVal->vt = VT_BSTR;
            break;
        }

        case UIA_IsContentElementPropertyId:
        case UIA_IsControlElementPropertyId:
            retVal->boolVal = VARIANT_TRUE;
            retVal->vt = VT_BOOL;
            break;

        case UIA_ControlTypePropertyId:
            retVal->vt = VT_I4;
            retVal->lVal = UIA_WindowControlTypeId;
            break;
        }

        return S_OK;
    }
    CATCH_RETURN();

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::get_HostRawElementProvider(
        _Outptr_ IRawElementProviderSimple** retVal) try
    {
        *retVal = nullptr;

        auto islandI = m_island.try_as<IContentIslandAccessibility>();
        if (islandI)
        {
            winrt::com_ptr<IUnknown> host;
            islandI->GetAutomationHostProvider(host.put());

            *retVal = host.as<IRawElementProviderSimple>().detach();
        }

        return S_OK;
    }
    CATCH_RETURN();

    winrt::com_ptr<IslandFragmentRoot>
    IslandFragmentRoot::GetPreviousSibling()
    {
        winrt::com_ptr<IslandFragmentRoot> previous = nullptr;
        if (m_parent != nullptr)
        {
            std::vector<winrt::com_ptr<IslandFragmentRoot>> siblings = m_parent->GetChildren();
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

    winrt::com_ptr<IslandFragmentRoot>
    IslandFragmentRoot::GetNextSibling()
    {
        winrt::com_ptr<IslandFragmentRoot> next = nullptr;
        if (m_parent != nullptr)
        {
            std::vector<winrt::com_ptr<IslandFragmentRoot>> siblings = m_parent->GetChildren();
            int index = GetCurrentIndexFromSiblings(siblings);

            if (index != -1)
            {
                if ((index+1) < static_cast<int>(siblings.size()))
                {
                    next = siblings[index + 1];
                }
            }
        }
        return next;
    }

    int
    IslandFragmentRoot::GetCurrentIndexFromSiblings(
        std::vector<winrt::com_ptr<IslandFragmentRoot>> siblings)
    {
        auto it = find_if(
                    siblings.begin(),
                    siblings.end(),
                    [this](const winrt::com_ptr<IslandFragmentRoot>& v) -> bool { return (v.get() == this); });

        int index = -1;
        if (it != siblings.end())
        {
            index = (int)(it - siblings.begin());
        }

        return index;
    }
 
    // IRawElementProviderFragment methods
    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::Navigate(
        NavigateDirection direction,
        _Outptr_result_maybenull_ IRawElementProviderFragment** retVal)
    {
        direction;
        *retVal = nullptr;

        winrt::com_ptr<IRawElementProviderFragment> element;

        switch (direction)
        {
        case NavigateDirection_Parent:
            if (m_parent)
            {
                element = m_parent.as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_FirstChild:
            if (!m_children.empty())
            {
                element = m_children.front().as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_LastChild:
            if (!m_children.empty())
            {
                element = m_children.back().as<IRawElementProviderFragment>();
            }
            break;

        case NavigateDirection_NextSibling:
        {
            winrt::com_ptr<IslandFragmentRoot> nextSibling = GetNextSibling();
            if (nextSibling)
            {
                element = nextSibling.as<IRawElementProviderFragment>();
            }
        }
        break;

        case NavigateDirection_PreviousSibling:
        {
            winrt::com_ptr<IslandFragmentRoot> previousSibling = GetPreviousSibling();
            if (previousSibling)
            {
                element = previousSibling.as<IRawElementProviderFragment>();
            }
            break;
        }
        }

        if (element)
        {

            *retVal = element.as<IRawElementProviderFragment>().detach();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::GetRuntimeId(
        _Outptr_ SAFEARRAY** retVal) try
    {
        *retVal = nullptr;
        std::array<int, 2> runtimeId = { UiaAppendRuntimeId, 0x1 };

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
    CATCH_RETURN();

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::get_BoundingRectangle(
        _Out_ UiaRect* retVal)
    {
        *retVal = {};
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::GetEmbeddedFragmentRoots(
        _Outptr_result_maybenull_ SAFEARRAY** retVal)
    {
        *retVal = nullptr;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::SetFocus()
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::get_FragmentRoot(
        _Outptr_ IRawElementProviderFragmentRoot** retVal)
    {
        *retVal = nullptr;
        RETURN_IF_FAILED(QueryInterface(IID_PPV_ARGS(retVal)));
        return S_OK;
    }

    // IRawElementProviderFragmentRoot methods
    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::ElementProviderFromPoint(
        double /* x */,
        double /* y */,
        _Outptr_result_maybenull_ IRawElementProviderFragment** retVal)
    {
        *retVal = nullptr;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    IslandFragmentRoot::GetFocus(
        _Outptr_result_maybenull_ IRawElementProviderFragment** retVal)
    {
        *retVal = nullptr;
        return S_OK;
    }

}
