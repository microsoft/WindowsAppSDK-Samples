// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "TextVisual.h"
#include "Output.h"
#include "TextRenderer.h"

#define FORCE_ALIASED_TEXT 0

template<class T>
TextVisual<T>::TextVisual(
        Output<T> const& output,
        std::wstring&& text,
        winrt::Windows::UI::Color const& backgroundColor,
        winrt::Windows::UI::Color const& textColor,
        IDWriteTextFormat* textFormat) :
    D2DSprite<T>(output),
    m_text(std::move(text)),
    m_textLayout(CreateTextLayout(m_text.c_str(), textFormat)),
    m_backgroundColor(backgroundColor),
    m_textColor(textColor)
{
    // Set the size of the container visual to the logical size of the text layout.
    D2DSprite<T>::GetVisual().Size(MeasureTextLayout(m_textLayout.get()));

    // Initialize the sprite visual.
    D2DSprite<T>::CreateDeviceDependentResources(output);
}

template<class T>
D2D1_RECT_F TextVisual<T>::GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform)
{
    if (m_backgroundColor.A != 0)
    {
        // The background is opaque, so compute the bounding box of the filled rectangle.
        auto size = ToSizeF(Measure());

        // The bounding box in logical units is { 0, 0, width, height }. Let p1..p4 be the
        // four vertices of this rectangle converted to raster units.
        auto p1 = rasterTransform.TransformPoint({0, 0});
        auto p2 = rasterTransform.TransformPoint({size.width, 0});
        auto p3 = rasterTransform.TransformPoint({size.width, size.height});
        auto p4 = rasterTransform.TransformPoint({0, size.height});

        // Return the bounding box.
        return {
            std::min(p1.x, std::min(p2.x, std::min(p3.x, p4.x))),
            std::min(p1.y, std::min(p2.y, std::min(p3.y, p4.y))),
            std::max(p1.x, std::max(p2.x, std::max(p3.x, p4.x))),
            std::max(p1.y, std::max(p2.y, std::max(p3.y, p4.y)))
        };
    }
    else
    {
        // The background is transparent so call the base implementation to compute the
        // bounds from the rendered primitives.
        return D2DSprite<T>::GetPixelBounds(output, rasterTransform);
    }
}

template<class T>
void TextVisual<T>::RenderContent(ID2D1DeviceContext5* deviceContext)
{
    // Create a brush, which we'll use for both the background (if any) and text.
    winrt::com_ptr<ID2D1SolidColorBrush> solidBrush;
    winrt::check_hresult(deviceContext->CreateSolidColorBrush(ToColorF(m_backgroundColor), solidBrush.put()));

    // Fill the background if it's not transparent.
    if (m_backgroundColor.A != 0)
    {
        auto logicalSize = MeasureTextLayout(m_textLayout.get());
        deviceContext->FillRectangle({0.0f, 0.0f, logicalSize.Width, logicalSize.Height}, solidBrush.get());
    }

#if FORCE_ALIASED_TEXT
    // Forcing aliased text rendering for debugging purposes. This makes it easy to tell if any
    // interpolation occurs during composition.
    deviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
#else
    // ClearType antialiasing is supported only if the background is fully opaque.
    deviceContext->SetTextAntialiasMode(
        m_backgroundColor.A == 0xFF ? D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE : D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
#endif

    // Draw the text layout object.
    solidBrush->SetColor(ToColorF(m_textColor));
    deviceContext->DrawTextLayout(D2D_POINT_2F{}, m_textLayout.get(), solidBrush.get());
}

template<class T>
winrt::Size TextVisual<T>::Measure() const
{
    return D2DSprite<T>::GetVisual().Size();
}

// Explicit template instantiation.
template class TextVisual<winrt::Compositor>;
template class TextVisual<winrt::WUC::Compositor>;
