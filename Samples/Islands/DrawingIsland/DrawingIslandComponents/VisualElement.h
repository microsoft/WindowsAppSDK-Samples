// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents::implementation
{
    class VisualElement
    {
    public:
        VisualElement(winrt::Compositor const& compositor);

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

    private:
        winrt::SpriteVisual m_visual;
    };
}
