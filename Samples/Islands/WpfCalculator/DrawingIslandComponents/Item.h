// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents::implementation
{
    // Abstract base class for an item that owns a SpriteVisual and can
    // re-rasterize the sprite if the DPI changes.
    class Item
    {
    public:
        Item(winrt::Compositor const& compositor);

        virtual ~Item()
        {
        }

        virtual void OnDpiScaleChanged()
        {
        }

        winrt::SpriteVisual const& GetVisual() const noexcept
        {
            return m_visual;
        }

    private:
        winrt::SpriteVisual m_visual;
    };
}
