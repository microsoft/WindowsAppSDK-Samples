// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "VisualElement.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents::implementation
{
    VisualElement::VisualElement(winrt::Compositor const& compositor) :
        m_visual(compositor.CreateSpriteVisual())
    {
    }
}
