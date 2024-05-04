#pragma once

#include "TextRenderer.h"
#include "NodeSimpleFragment.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextBlock
    {
    public:
        TextBlock(
            std::shared_ptr<TextRenderer> const& textRenderer,
#if TRUE
            winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
            winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
#endif
            CompositionColorBrush visualBrush,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            std::wstring&& text
        );

        winrt::SpriteVisual const& GetVisual() const noexcept
        {
            return m_visual;
        }

        winrt::com_ptr<NodeSimpleFragment> const& GetFragment() const noexcept
        {
            return m_fragment;
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

#if TRUE
        winrt::com_ptr<NodeSimpleFragment> m_fragment;
#endif
    };
}
