// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextItem.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextItem::TextItem(
        std::shared_ptr<TextRenderer> const& textRenderer,
        Windows::UI::Color backgroundColor,
        Windows::UI::Color textColor,
        std::wstring const& text) :
        Item(textRenderer->GetCompositor()),
        m_textRenderer(textRenderer),
        m_backgroundColor(backgroundColor),
        m_textColor(textColor),
        m_text(text)
    {
        InitializeVisual();
    }

    void TextItem::InitializeVisual()
    {
        // Render the text to the sprite visual.
        m_textRenderer->Render(m_text.c_str(), m_backgroundColor, m_textColor, GetVisual());
    }
}
