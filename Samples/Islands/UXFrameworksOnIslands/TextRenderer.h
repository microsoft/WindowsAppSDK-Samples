// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

extern winrt::com_ptr<IDWriteFactory7> const g_dwriteFactory;

inline constexpr float k_defaultFontSize = 14.0f;
inline constexpr float k_headingFontSize = 16.0f;

// Creates an object that encapsulates text formatting properties.
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(
    _In_z_ WCHAR const* familyName,
    float fontSize = k_defaultFontSize,
    DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL);

// Creates an object that represents a formatted text string.
winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(_In_z_ WCHAR const* text, _In_opt_ IDWriteTextFormat* textFormat = nullptr);

// Computes the logical width and height of the text layout object.
// Note: These are layout bounds, not ink bounds.
winrt::Size MeasureTextLayout(IDWriteTextLayout* textLayout);

// Creates the default "Normal" font face in the specified font family.
winrt::com_ptr<IDWriteFontFace3> CreateFontFace(_In_z_ wchar_t const* familyName);

// Specialized class for rendering decimal numbers.
//
// Since we're only rendering the digits '0' through '9' in a known font,
// we can optimize by making some assumptions that would not be safe for
// arbitrary text, namely:
//
//  -  All the characters we need to render are in the base font.
//  -  The text is a 1:1 mapping of characters to glyphs (no complex layout).
//  -  All glyphs have the same advance width.
//  -  The height only includes the cap height (no descent or extra white space).
//  
class DigitRenderer
{
public:
    explicit DigitRenderer(float fontSize);

    class DigitGlyphs
    {
    public:
        // UINT32_MAX has 10 decimal digits.
        static constexpr uint32_t k_maxLength = 10;

        DigitGlyphs(uint16_t const (&digitGlyphIds)[10], uint32_t value) noexcept;

        _Ret_range_(1, k_maxLength) uint32_t Length() const noexcept
        {
            return m_length;
        }

        _Ret_writes_(m_length) uint16_t const* GlyphIds() const noexcept
        {
            return m_glyphIds + (k_maxLength - m_length);
        }

    private:
        // Buffer for glyph IDs. We grow from the end, so the glyph IDs
        // are in the last m_length elements of this buffer.
        uint16_t m_glyphIds[k_maxLength];

        // Number of digits.
        _Field_range_(1, k_maxLength) uint32_t m_length;
    };

    DigitGlyphs GetDigitGlyphs(uint32_t value) const noexcept
    {
        return DigitGlyphs(m_digitGlyphIds, value);
    }

    float Width(DigitGlyphs const& digits) const noexcept
    {
        return m_scaledAdvance * digits.Length();
    }

    float Height() const noexcept
    {
        return m_scaledHeight;
    }

    void Draw(ID2D1DeviceContext* deviceContext, D2D_POINT_2F const& topLeft, DigitGlyphs const& digits, ID2D1Brush* brush) const;

private:
    winrt::com_ptr<IDWriteFontFace3> m_fontFace;
    float m_fontSize;
    float m_scaledHeight;
    float m_scaledAdvance;
    uint16_t m_digitGlyphIds[10];
};
