// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextRenderer.h"
#include "VisualElement.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextElement final : public VisualElement
    {
    public:
        TextElement(
#if TRUE
            winrt::com_ptr<NodeSimpleFragmentFactory> const& fragmentFactory,
            winrt::com_ptr<IslandFragmentRoot> const& fragmentRoot,
#endif
            std::shared_ptr<TextRenderer> const& textRenderer,
            CompositionColorBrush visualBrush,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            std::wstring const& text
        );

        void OnDpiScaleChanged() override;

    private:
        std::shared_ptr<TextRenderer> m_textRenderer;
        Windows::UI::Color m_backgroundColor;
        Windows::UI::Color m_textColor;
        std::wstring m_text;
    };
}
