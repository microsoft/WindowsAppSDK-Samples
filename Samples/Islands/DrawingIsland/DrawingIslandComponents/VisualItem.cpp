// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "VisualItem.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents::implementation
{
    VisualItem::VisualItem(winrt::Compositor const& compositor) :
        m_visual(compositor.CreateSpriteVisual())
    {
    }
}
