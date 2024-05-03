#pragma once

#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextBlock
    {
    public:
        TextBlock(winrt::com_ptr<TextRenderer> const& textRenderer);

    private:
        winrt::com_ptr<TextRenderer> m_textRenderer;
    };
}
