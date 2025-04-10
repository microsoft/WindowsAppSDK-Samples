// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "TextRenderer.h"
#include "CompositionDeviceResource.h"

namespace
{
    winrt::com_ptr<IDWriteFactory7> CreateDWriteFactory()
    {
        winrt::com_ptr<IDWriteFactory7> factory;

        // Create the DWrite factory object.
        winrt::check_hresult(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(decltype(*factory)),
            reinterpret_cast<IUnknown**>(factory.put())));

        return factory;
    }
}

winrt::com_ptr<IDWriteFactory7> const g_dwriteFactory = CreateDWriteFactory();

// Creates an object that encapsulates text formatting properties.
winrt::com_ptr<IDWriteTextFormat> CreateTextFormat(
    _In_z_ WCHAR const* familyName,
    float fontSize,
    DWRITE_FONT_WEIGHT fontWeight,
    DWRITE_FONT_STYLE fontStyle,
    DWRITE_FONT_STRETCH fontStretch)
{
    winrt::com_ptr<IDWriteTextFormat> textFormat;

    winrt::check_hresult(g_dwriteFactory->CreateTextFormat(
        familyName,
        nullptr,
        fontWeight,
        fontStyle,
        fontStretch,
        fontSize,
        L"en-US",
        textFormat.put()));

    winrt::check_hresult(textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

    return textFormat;
}

winrt::com_ptr<IDWriteTextFormat> const g_defaultTextFormat = CreateTextFormat(L"Segoe UI", k_defaultFontSize);

// Creates an object that represents a formatted text string.
winrt::com_ptr<IDWriteTextLayout> CreateTextLayout(_In_z_ WCHAR const* text, _In_opt_ IDWriteTextFormat* textFormat)
{
    winrt::com_ptr<IDWriteTextLayout> textLayout;
    winrt::check_hresult(g_dwriteFactory->CreateTextLayout(
        text,
        static_cast<uint32_t>(wcslen(text)),
        textFormat != nullptr ? textFormat : g_defaultTextFormat.get(),
        /*maxWidth*/ 0,
        /*maxHeight*/ 0,
        /*out*/ textLayout.put()));
    return textLayout;
}

// Computes the logical width and height of the text layout object.
// Note: These are layout bounds, not ink bounds.
winrt::Size MeasureTextLayout(IDWriteTextLayout* textLayout)
{
    // Get the metrics from the text layout.
    DWRITE_TEXT_METRICS textMetrics;
    winrt::check_hresult(textLayout->GetMetrics(/*out*/ &textMetrics));

    return { textMetrics.width, textMetrics.height };
}

winrt::com_ptr<IDWriteFontFace3> CreateFontFace(_In_z_ wchar_t const* familyName)
{
    winrt::com_ptr<IDWriteFontCollection3> fontCollection;
    winrt::check_hresult(g_dwriteFactory->GetSystemFontCollection(
        false,
        DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC,
        fontCollection.put()));

    static const DWRITE_FONT_AXIS_VALUE axisValues[] = {
        { DWRITE_FONT_AXIS_TAG_WEIGHT, 400 },
        { DWRITE_FONT_AXIS_TAG_WIDTH, 100 },
        { DWRITE_FONT_AXIS_TAG_ITALIC, 0 },
        { DWRITE_FONT_AXIS_TAG_SLANT, 0 }
    };

    winrt::com_ptr<IDWriteFontList2> matchingFonts;
    winrt::check_hresult(fontCollection->GetMatchingFonts(
        familyName,
        axisValues,
        ARRAYSIZE(axisValues),
        matchingFonts.put()));

    winrt::com_ptr<IDWriteFontFaceReference> faceReference;
    winrt::check_hresult(matchingFonts->GetFontFaceReference(0, faceReference.put()));

    winrt::com_ptr<IDWriteFontFace3> fontFace;
    winrt::check_hresult(faceReference->CreateFontFace(fontFace.put()));

    return fontFace;
}

DigitRenderer::DigitRenderer(float fontSize) :
    m_fontFace(CreateFontFace(L"Segoe UI")),
    m_fontSize(fontSize)
{
    // Get the glyph IDs for digits '0' through '9'.
    uint32_t digitChars[10];
    for (uint32_t i = 0; i < 10; i++)
    {
        digitChars[i] = L'0' + i;
    }
    winrt::check_hresult(m_fontFace->GetGlyphIndices(digitChars, 10, m_digitGlyphIds));

    // Get the conversion factor from font design units to logical units (DIPs).
    DWRITE_FONT_METRICS fontMetrics;
    m_fontFace->GetMetrics(&fontMetrics);
    const float designToDips = m_fontSize / fontMetrics.designUnitsPerEm;

    // Get the cap height, which is also the height of a digit in Segoe UI.
    m_scaledHeight = fontMetrics.capHeight * designToDips;

    // Get the advance width, assume all digits are the same with as '0'.
    int32_t designAdvance;
    winrt::check_hresult(m_fontFace->GetDesignGlyphAdvances(1, m_digitGlyphIds, &designAdvance));
    m_scaledAdvance = designAdvance * designToDips;
}

DigitRenderer::DigitGlyphs::DigitGlyphs(uint16_t const (&digitGlyphIds)[10], uint32_t value) noexcept
{
    // Fill the m_glyphIds buffer from back to front.
    uint32_t firstIndex = k_maxLength;

    // Fill in the glyph ID for the last digit.
    m_glyphIds[--firstIndex] = digitGlyphIds[value % 10];
    value /= 10;

    // Fill in the glyph IDs for the preceding digits.
    while (firstIndex > 0 && value != 0)
    {
        m_glyphIds[--firstIndex] = digitGlyphIds[value % 10];
        value /= 10;
    }

    m_length = k_maxLength - firstIndex;
}

void DigitRenderer::Draw(ID2D1DeviceContext* deviceContext, D2D_POINT_2F const& topLeft, DigitGlyphs const& digits, ID2D1Brush* brush) const
{
    // Initialize the array of glyph Advances.
    float runAdvances[DigitGlyphs::k_maxLength];
    for (uint32_t i = 0; i < digits.Length(); i++)
    {
        runAdvances[i] = m_scaledAdvance;
    }

    // Fill in the glyph run structure.
    DWRITE_GLYPH_RUN glyphRun{};
    glyphRun.fontFace = m_fontFace.get();
    glyphRun.fontEmSize = m_fontSize;
    glyphRun.glyphCount = digits.Length();
    glyphRun.glyphIndices = digits.GlyphIds();
    glyphRun.glyphAdvances = runAdvances;

    // Draw the glyph run.
    D2D_POINT_2F baselineOrigin{topLeft.x, topLeft.y + m_scaledHeight};
    deviceContext->DrawGlyphRun(baselineOrigin, &glyphRun, brush);
}
