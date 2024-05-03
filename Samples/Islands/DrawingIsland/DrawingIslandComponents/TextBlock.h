#pragma once

#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextBlock
    {
    public:
        TextBlock(
            winrt::Compositor compositor,
            std::shared_ptr<TextRenderer> const& textRenderer,
            std::wstring&& text
        );

        float2 GetSize() const noexcept
        {
            return m_size;
        }

    private:
        std::shared_ptr<TextRenderer> m_textRenderer;
        std::wstring m_text;
        float2 m_size;
    };
}
