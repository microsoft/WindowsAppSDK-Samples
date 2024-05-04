// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "VisualElement.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents::implementation
{
    VisualElement::VisualElement(
#if TRUE
        winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
        winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
        _In_z_ wchar_t const* name,
#endif
        winrt::Compositor const& compositor
    ) :
        m_visual(compositor.CreateSpriteVisual())
    {
#if TRUE
        m_fragment = fragmentFactory->Create(name, fragmentRoot);

        m_fragment->SetVisual(m_visual);

        // Set up children roots
        fragmentRoot->AddChild(m_fragment);

        // Finally set up parent chain
        m_fragment->SetParent(fragmentRoot);
#endif
    }
}
