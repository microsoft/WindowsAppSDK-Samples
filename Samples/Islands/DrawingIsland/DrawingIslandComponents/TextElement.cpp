// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextElement.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextElement::TextElement(
#if TRUE
        winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
        winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
#endif
        std::shared_ptr<TextRenderer> const& textRenderer,
        CompositionColorBrush visualBrush,
        Windows::UI::Color backgroundColor,
        Windows::UI::Color textColor,
        std::wstring const& text
    ) :
        VisualElement(
#if TRUE
            fragmentFactory,
            fragmentRoot,
            text.c_str(),
#endif
            textRenderer->GetCompositor()
        ),
        m_textRenderer(textRenderer),
        m_backgroundColor(backgroundColor),
        m_textColor(textColor),
        m_text(text)
    {
        // Set the visual brush.
        // TODO - this will go away when we implement text
        GetVisual().Brush(visualBrush);

        // Render the text to determine the size.
        m_textRenderer->Render(m_text.c_str(), GetVisual());
    }

    void TextElement::OnDpiScaleChanged()
    {
        // TODO - re-render the text
    }
}
