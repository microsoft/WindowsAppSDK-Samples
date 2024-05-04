// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "NodeSimpleFragment.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class VisualElement
    {
    public:
        VisualElement(
#if TRUE
            winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
            winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
            _In_z_ wchar_t const* name,
#endif
            winrt::Compositor const& compositor
        );

        virtual ~VisualElement()
        {
        }

        virtual void OnDpiScaleChanged()
        {
        }

        winrt::SpriteVisual const& GetVisual() const noexcept
        {
            return m_visual;
        }

        winrt::com_ptr<NodeSimpleFragment> const& GetFragment() const noexcept
        {
            return m_fragment;
        }

    private:
        winrt::SpriteVisual m_visual;
#if TRUE
        winrt::com_ptr<NodeSimpleFragment> m_fragment;
#endif
    };
}
