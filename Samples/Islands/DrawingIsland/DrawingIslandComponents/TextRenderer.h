// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents::implementation
{
    class TextRenderer final
    {
    public:
        TextRenderer(winrt::Compositor compositor);

        void SetDpiScale(float scale)
        {
            // TODO
            m_dpiScale = scale;
        }

        void SetBackgroundColor(D2D_COLOR_F color) noexcept
        {
            m_backgroundColor = color;
        }

        void SetTextColor(D2D_COLOR_F color) noexcept
        {
            m_textColor = color;
        }

        void Render(_In_z_ WCHAR const* text);

        float2 GetSize() const noexcept
        {
            return m_size;
        }

        winrt::Compositor const& GetCompositor() const noexcept
        {
            return m_compositor;
        }

    private:
        winrt::Compositor m_compositor;

        float m_dpiScale = 1.0f;
        D2D_COLOR_F m_backgroundColor = { 1, 1, 1, 1 };
        D2D_COLOR_F m_textColor = { 0, 0, 0, 1 };

        // DWrite API objects.
        winrt::com_ptr<IDWriteFactory7> m_dwriteFactory;
        winrt::com_ptr<IDWriteTextFormat> m_textFormat;

        // Size of the text in DIPs and in pixels.
        float2 m_size = {};
    };
}
