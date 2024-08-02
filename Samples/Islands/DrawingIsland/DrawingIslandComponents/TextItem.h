// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextRenderer.h"
#include "Item.h"

namespace winrt::DrawingIslandComponents::implementation
{
    class TextItem final : public Item
    {
    public:
        TextItem(
            std::shared_ptr<TextRenderer> const& textRenderer,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            std::wstring const& text);

        void OnDpiScaleChanged() override
        {
            // Re-render the text using the current DPI scale.
            InitializeVisual();
        }

        void OnDeviceLost() override
        {
            // Re-render the text using the current device.
            InitializeVisual();
        }

    private:
        void InitializeVisual();

        std::shared_ptr<TextRenderer> m_textRenderer;
        Windows::UI::Color m_backgroundColor;
        Windows::UI::Color m_textColor;
        std::wstring m_text;
    };
}
