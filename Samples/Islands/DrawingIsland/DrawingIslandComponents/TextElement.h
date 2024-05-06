﻿// Copyright (c) Microsoft Corporation.
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
            std::shared_ptr<TextRenderer> const& textRenderer,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            std::wstring const& text
        );

        void OnDpiScaleChanged() override;

    private:
        void InitializeVisual();

        std::shared_ptr<TextRenderer> m_textRenderer;
        Windows::UI::Color m_backgroundColor;
        Windows::UI::Color m_textColor;
        std::wstring m_text;
    };
}
