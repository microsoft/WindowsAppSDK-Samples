// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextBlock.h"
#include "IslandFragmentRoot.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextBlock::TextBlock(
        std::shared_ptr<TextRenderer> const& textRenderer,
#if TRUE
        winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
        winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
#endif
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

#if TRUE
        m_fragment = fragmentFactory->Create(m_text.c_str(), fragmentRoot);

        m_fragment->SetVisual(m_visual);

        // Set up children roots
        fragmentRoot->AddChild(m_fragment);

        // Finally set up parent chain
        m_fragment->SetParent(fragmentRoot);
#endif
    }
}
