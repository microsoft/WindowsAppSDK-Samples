#pragma once

#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextBlock
    {
    public:
        TextBlock(
            std::shared_ptr<TextRenderer> const& textRenderer,
            CompositionColorBrush visualBrush,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            std::wstring&& text
        );

        winrt::SpriteVisual const& GetVisual() const noexcept
        {
            return m_visual;
        }

        float2 GetSize() const noexcept
        {
            return m_size;
        }

    private:
        std::shared_ptr<TextRenderer> m_textRenderer;
        Windows::UI::Color m_backgroundColor;
        Windows::UI::Color m_textColor;
        std::wstring m_text;
        float2 m_size;
        winrt::SpriteVisual m_visual;
    };
}
