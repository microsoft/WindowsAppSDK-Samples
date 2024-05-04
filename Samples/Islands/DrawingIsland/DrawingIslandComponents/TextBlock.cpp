// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextBlock.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextBlock::TextBlock(
        std::shared_ptr<TextRenderer> const& textRenderer,
        CompositionColorBrush visualBrush,
        Windows::UI::Color backgroundColor,
        Windows::UI::Color textColor,
        std::wstring&& text
    ) :
        m_textRenderer(textRenderer),
        m_backgroundColor(backgroundColor),
        m_textColor(textColor),
        m_text(std::move(text)),
        m_visual(m_textRenderer->GetCompositor().CreateSpriteVisual())
    {
        // Render the text to determine the size.
        // TODO - this should create a composition brush
        m_textRenderer->Render(m_text.c_str());
        m_size = m_textRenderer->GetSize();

        // Set the visual size and brush.
        m_visual.Brush(visualBrush);
        m_visual.Size(m_size);
    }
}
