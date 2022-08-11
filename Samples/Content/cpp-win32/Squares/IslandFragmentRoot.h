// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Squares
{
    class NodeSimpleFragment;

    class IslandFragmentRoot  :
        public winrt::implements<IslandFragmentRoot,
                                 IRawElementProviderSimple,
                                 IRawElementProviderFragment,
                                 IRawElementProviderFragmentRoot,
                                 IInspectable>
    {
    public:
        explicit IslandFragmentRoot(
            winrt::ContentIsland island) : m_island(island) {}

        void AddChild(
            winrt::com_ptr<NodeSimpleFragment> child);

        std::vector<winrt::com_ptr<NodeSimpleFragment>> GetChildren();

        void SetParent(
            winrt::com_ptr<NodeSimpleFragment> parent);

        void SetName(
            std::wstring name);

        // IRawElementProviderSimple methods
        HRESULT STDMETHODCALLTYPE get_ProviderOptions(
            _Out_ ProviderOptions* retVal) override final;

        HRESULT STDMETHODCALLTYPE GetPatternProvider(
            PATTERNID /* idPattern */,
            _Outptr_result_maybenull_ IUnknown** retVal) override final;

        HRESULT STDMETHODCALLTYPE GetPropertyValue(
            PROPERTYID idProp,
            _Out_ VARIANT* retVal) override final;

        HRESULT STDMETHODCALLTYPE get_HostRawElementProvider(
            _Outptr_ IRawElementProviderSimple** retVal) override final;
 
        // IRawElementProviderFragment methods
        HRESULT STDMETHODCALLTYPE Navigate(
            NavigateDirection direction,
            _Outptr_result_maybenull_ IRawElementProviderFragment** retVal) override final;

        HRESULT STDMETHODCALLTYPE GetRuntimeId(
            _Outptr_ SAFEARRAY** retVal) override final;

        HRESULT STDMETHODCALLTYPE get_BoundingRectangle(
            _Out_ UiaRect* retVal) override final;

        HRESULT STDMETHODCALLTYPE GetEmbeddedFragmentRoots(
            _Outptr_result_maybenull_ SAFEARRAY** retVal) override final;

        HRESULT STDMETHODCALLTYPE SetFocus() override final;

        HRESULT STDMETHODCALLTYPE get_FragmentRoot(
            _Outptr_ IRawElementProviderFragmentRoot** retVal) override final;

        // IRawElementProviderFragmentRoot methods
        HRESULT STDMETHODCALLTYPE ElementProviderFromPoint(
            double /* x */,
            double /* y */,
            _Outptr_result_maybenull_ IRawElementProviderFragment** retVal) override final;

        HRESULT STDMETHODCALLTYPE GetFocus(
            _Outptr_result_maybenull_ IRawElementProviderFragment** retVal) override final;

    private:
        // Helper methods
        winrt::com_ptr<IslandFragmentRoot> GetPreviousSibling();

        winrt::com_ptr<IslandFragmentRoot> GetNextSibling();

        int GetCurrentIndexFromSiblings(
            std::vector<winrt::com_ptr<IslandFragmentRoot>> siblings);

    private:
        HWND m_hwnd{};
        winrt::ContentIsland m_island{ nullptr };
        std::wstring m_name;
        std::vector<winrt::com_ptr<NodeSimpleFragment>> m_children;
        winrt::com_ptr<NodeSimpleFragment> m_parent;
    };
}
