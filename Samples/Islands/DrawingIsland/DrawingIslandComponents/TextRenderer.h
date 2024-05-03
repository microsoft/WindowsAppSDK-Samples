// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <dwrite_core.h>

class TextRenderer final : public IDWriteTextRenderer
{
public:
    // IUnknown method
    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        _COM_Outptr_ void** ppvObject
        ) override;

    // IUnknown method
    ULONG STDMETHODCALLTYPE AddRef() override;

    // IUnknown method
    ULONG STDMETHODCALLTYPE Release() override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(
        _In_opt_ void* clientDrawingContext,
        _Out_ BOOL* isDisabled
        ) override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE GetCurrentTransform(
        _In_opt_ void* clientDrawingContext,
        _Out_ DWRITE_MATRIX* transform
        ) override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE GetPixelsPerDip(
        _In_opt_ void* clientDrawingContext,
        _Out_ FLOAT* pixelsPerDip
        ) override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawGlyphRun(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        _In_ DWRITE_GLYPH_RUN const* glyphRun,
        _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawUnderline(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_UNDERLINE const* underline,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawStrikethrough(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_STRIKETHROUGH const* strikethrough,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawInlineObject(
        _In_opt_ void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        _In_ IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override;

    static winrt::com_ptr<TextRenderer> Create()
    {
        return winrt::com_ptr<TextRenderer>(new TextRenderer(), winrt::take_ownership_from_abi);
    }

    void SetDpiScale(float scale)
    {
        m_dpiScale = scale;
    }

    // Convert GDI COLORREF value (from RGB macro) to BGRA32.
    static constexpr uint32_t Bgra32FromColorRef(COLORREF color) noexcept
    {
        // Swap the red and blue channel (bytes 1 and 3) and set the alpha to 0xFF.
        return
            ((color & 0x0000FF) << 16) |    // b
            (color & 0x00FF00) |            // g
            ((color & 0xFF0000) >> 16) |    // r
            0xFF000000;                     // a
    }

    void SetBackgroundColor(COLORREF color) noexcept
    {
        m_backgroundColor = Bgra32FromColorRef(color);
    }

    void SetTextColor(COLORREF color) noexcept
    {
        m_textColor = color;
    }

    void Render(_In_z_ WCHAR const* text);

    void Resize(uint32_t pixelWidth, uint32_t pixelHeight);

    void ClearBackground() noexcept;

    DWRITE_BITMAP_DATA_BGRA32 const& GetBitapData() const noexcept
    {
        return m_bitmapData;
    }

private:
    TextRenderer();

    // The reference count is intialized to 1 because Create invokes a winrt::com_ptr method
    // that assumes ownership *without* incrementing the reference count.
    LONG m_refCount = 1;

    float m_dpiScale = 1.0f;
    uint32_t m_backgroundColor = Bgra32FromColorRef(RGB(64, 64, 128));
    uint32_t m_textColor = RGB(0, 0, 0);

    // DWrite API objects.
    winrt::com_ptr<IDWriteFactory8> m_dwriteFactory;
    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    winrt::com_ptr<IDWriteBitmapRenderTarget3> m_renderTarget;
    DWRITE_BITMAP_DATA_BGRA32 m_bitmapData;
    winrt::com_ptr<IDWriteRenderingParams> m_renderingParams;
};
