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

        void Render(_In_z_ WCHAR const* text, SpriteVisual const& visual);

        winrt::Compositor const& GetCompositor() const noexcept
        {
            return m_compositor;
        }

    private:
        winrt::Compositor m_compositor;

        float m_dpiScale = 1.0f;

        // DWrite API objects.
        winrt::com_ptr<IDWriteFactory7> m_dwriteFactory;
        winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    };
}
