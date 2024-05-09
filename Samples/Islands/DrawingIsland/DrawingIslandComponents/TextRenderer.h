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
            m_dpiScale = scale;
        }

        void Render(
            _In_z_ WCHAR const* text,
            Windows::UI::Color backgroundColor,
            Windows::UI::Color textColor,
            SpriteVisual const& visual);

        winrt::Compositor const& GetCompositor() const noexcept
        {
            return m_compositor;
        }

        winrt::com_ptr<::IDXGIDevice> const& GetDevice() const noexcept
        {
            return m_dxgiDevice;
        }

        void RecreateDirect2DDevice();

    private:
        static inline D2D_COLOR_F ToColorF(Windows::UI::Color color)
        {
            return D2D_COLOR_F{ color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
        }

        void CreateDirect2DDevice();

        winrt::Compositor m_compositor;

        // Device-independent resources.
        winrt::com_ptr<::IDWriteFactory7> m_dwriteFactory;
        winrt::com_ptr<::IDWriteTextFormat> m_textFormat;
        winrt::com_ptr<::ID2D1Factory7> m_d2dFactory;

        // Device-dependent resources.
        winrt::com_ptr<::IDXGIDevice> m_dxgiDevice;
        winrt::com_ptr<::ID2D1Device6> m_d2dDevice;
        CompositionGraphicsDevice m_compositionGraphicsDevice{ nullptr };

        float m_dpiScale = 1.0f;
    };
}
