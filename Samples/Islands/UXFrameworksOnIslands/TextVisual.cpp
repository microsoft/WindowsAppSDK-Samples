// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "TextVisual.h"
#include "Output.h"
#include "TextRenderer.h"

template<class T>
TextVisual<T>::TextVisual(Output<T> const& output, std::wstring&& text) :
    TextVisual(output, std::move(text), nullptr)
{
}

template<class T>
TextVisual<T>::TextVisual(
        Output<T> const& output,
        std::wstring&& text,
        winrt::com_ptr<IDWriteTextLayout> const& textLayout,
        ContainerVisual const& containerVisual) :
    D2DSprite<T>(output, containerVisual),
    m_text(std::move(text)),
    m_textLayout(textLayout != nullptr ? textLayout : CreateTextLayout(m_text.c_str())),
    m_size(MeasureTextLayout(m_textLayout.get()))
{
    SetVisualSize();
}

template<class T>
void TextVisual<T>::SetVisualSize()
{
    GetVisual().Size({ m_origin.x + m_size.Width, m_origin.y + m_size.Height });
}

template<class T>
D2D_RECT_F TextVisual<T>::GetLogicalBounds() const noexcept
{
    return { m_origin.x, m_origin.y, m_origin.x + m_size.Width, m_origin.y + m_size.Height };
}

template<class T>
D2D1_RECT_F TextVisual<T>::GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform)
{
    if (m_size.Width == 0 || m_size.Height == 0)
    {
        // Empty text layout.
        return {};
    }
    else
    {
        // Call the base implementation to compute the bounds from the rendered primitives.
        return D2DSprite<T>::GetPixelBounds(output, rasterTransform);
    }
}

template<class T>
void TextVisual<T>::RenderContent(Output<T> const& output, ID2D1DeviceContext5* deviceContext)
{
    // Set the text antialiasing mode.
    // Note that ClearType is only used if the background is opaque.
    auto antialiasMode = output.GetSetting(Setting_ForceAliasedText) ? D2D1_TEXT_ANTIALIAS_MODE_ALIASED :
        GetBackgroundColor().A == 0xFF ? D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE :
        D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
    deviceContext->SetTextAntialiasMode(antialiasMode);

    // Create the text brush.
    winrt::com_ptr<ID2D1SolidColorBrush> solidBrush;
    winrt::check_hresult(deviceContext->CreateSolidColorBrush(ToColorF(m_textColor), solidBrush.put()));

    // Draw the text layout object.
    deviceContext->DrawTextLayout({ m_origin.x, m_origin.y }, m_textLayout.get(), solidBrush.get());
}

template<class T>
void TextVisual<T>::SetTextOrigin(winrt::Windows::Foundation::Numerics::float2 origin)
{
    m_origin = origin;
    SetVisualSize();
}

template<class T>
void TextVisual<T>::SetTextColor(winrt::Windows::UI::Color const& color)
{
    if (color != m_textColor)
    {
        m_textColor = color;
        InvalidateContent();
    }
}

template<class T>
void TextVisual<T>::SetFormatWidth(float width)
{
    if (m_textLayout->GetWordWrapping() != DWRITE_WORD_WRAPPING_NO_WRAP)
    {
        m_textLayout->SetMaxWidth(width);
        m_size = MeasureTextLayout(m_textLayout.get());
        SetVisualSize();
        InvalidateContent();
    }
}

// Explicit template instantiation.
template class TextVisual<winrt::Compositor>;
template class TextVisual<winrt::WUC::Compositor>;
