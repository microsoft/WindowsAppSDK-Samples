// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextBlock.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextBlock::TextBlock(
        winrt::Compositor compositor,
        std::shared_ptr<TextRenderer> const& textRenderer,
        std::wstring&& text
    ) :
        m_textRenderer(textRenderer),
        m_text(std::move(text))
    {
        m_textRenderer->Render(m_text.c_str());
        m_size = m_textRenderer->GetSize();

        // TODO
    }
}
