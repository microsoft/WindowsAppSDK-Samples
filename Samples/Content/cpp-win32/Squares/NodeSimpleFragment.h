// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Squares
{
    class IslandFragmentRoot;

    class NodeSimpleFragment :
        public winrt::implements<NodeSimpleFragment,
                                 IRawElementProviderSimple,
                                 IRawElementProviderFragment>
    {
    public:
        NodeSimpleFragment(
            _In_z_ const wchar_t* name,
            int id);

        void AddChild(winrt::com_ptr<IslandFragmentRoot> child);

        std::vector<winrt::com_ptr<IslandFragmentRoot>> GetChildren();

        void SetId(
            int id);

        void SetBoundingRects(
            UiaRect rect);

        void SetName(
            std::wstring name);

        void SetParent(
            winrt::com_ptr<IslandFragmentRoot> parent);

        // IRawElementProviderSimple methods
        IFACEMETHODIMP get_ProviderOptions(
            _Out_ ProviderOptions* retVal) override final;

        IFACEMETHODIMP GetPatternProvider(
            PATTERNID patternId,
            _Outptr_ IUnknown** retVal) override final;

        IFACEMETHODIMP GetPropertyValue(
            PROPERTYID propertyId,
            _Out_ VARIANT* retVal) override final;

        IFACEMETHODIMP get_HostRawElementProvider(
            _Outptr_ IRawElementProviderSimple** retVal) override final;

        // IRawElementProviderFragment methods
        IFACEMETHODIMP Navigate(
            _In_ NavigateDirection direction,
            _Outptr_ IRawElementProviderFragment** retVal) override final;

        IFACEMETHODIMP GetRuntimeId(
            _Outptr_ SAFEARRAY** retVal) override final;

        IFACEMETHODIMP get_BoundingRectangle(
            _Out_ UiaRect* retVal) override final;

        IFACEMETHODIMP GetEmbeddedFragmentRoots(
            _Outptr_ SAFEARRAY** retVal) override final;

        IFACEMETHODIMP SetFocus();

        IFACEMETHODIMP get_FragmentRoot(
            _Outptr_ IRawElementProviderFragmentRoot** retVal) override final;

    private:
        // Helper methods
        // For the below 3 methods create a generic function instead so that they can be reused.
        winrt::com_ptr<NodeSimpleFragment> GetPreviousSibling();

        winrt::com_ptr<NodeSimpleFragment> GetNextSibling();

        int GetCurrentIndexFromSiblings(
            std::vector<winrt::com_ptr<NodeSimpleFragment>> siblings);

    private:
        winrt::com_ptr<IslandFragmentRoot> m_parent;
        std::vector<winrt::com_ptr<IslandFragmentRoot>> m_children;

        int m_id = 0xff;
        std::wstring m_name;
        UiaRect m_boundingRect = { 0, 0, 0, 0 };
    };

    // A helper class that creates Fragments that are part of the same hierarchy. Most importantly,
    // it ensures that each NodeSimpleFragment gets assigned a unique ID.
    class NodeSimpleFragmentFactory:
        public winrt::implements<NodeSimpleFragmentFactory, winrt::IInspectable>
    {
    public:
        winrt::com_ptr<NodeSimpleFragment> Create(
            _In_z_ const wchar_t* name,
            winrt::com_ptr<IslandFragmentRoot> fragmentRoot)
        {
            return winrt::make_self<NodeSimpleFragment>(name, ++m_nextId);
        }

    private:
        int m_nextId = 10;
    };
}
