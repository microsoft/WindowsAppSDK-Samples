// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Item.h"

namespace winrt::DrawingIslandComponents::implementation
{
    Item::Item(winrt::Compositor const& compositor) :
        m_visual(compositor.CreateSpriteVisual())
    {
    }
}
