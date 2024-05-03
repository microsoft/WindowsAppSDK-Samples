// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextRenderer::TextRenderer()
    {
        // Create the DWrite factory object.
        THROW_IF_FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(decltype(*m_dwriteFactory)),
            reinterpret_cast<::IUnknown**>(m_dwriteFactory.put())
        ));

        // Create an object that encapsulates text formatting properties.
        constexpr float defaultFontSize = 16;
        THROW_IF_FAILED(m_dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            defaultFontSize,
            L"en-US",
            m_textFormat.put()
        ));
        THROW_IF_FAILED(m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        // TODO - create D2D objects
    }

    void TextRenderer::Render(_In_z_ WCHAR const* text)
    {
        winrt::com_ptr<IDWriteTextLayout> textLayout;
        THROW_IF_FAILED(m_dwriteFactory->CreateTextLayout(
            text,
            static_cast<uint32_t>(wcslen(text)),
            m_textFormat.get(),
            /*maxWidth*/ 0,
            /*maxHeight*/ 0,
            /*out*/ textLayout.put()
        ));

        DWRITE_TEXT_METRICS textMetrics;
        THROW_IF_FAILED(textLayout->GetMetrics(/*out*/ &textMetrics));

        m_size = { textMetrics.width, textMetrics.height };

        // TODO - draw the text
    }
}
