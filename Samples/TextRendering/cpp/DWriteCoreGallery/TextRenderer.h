// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

class TextRenderer final : public IDWriteTextRenderer1
{
public:
    explicit TextRenderer(HWND hwnd);

    void Resize(SIZE pixelSize);
    void Clear(int sysColorIndex);
    void SetTextColor(int sysColorIndex) { m_textColor = GetSysColor(sysColorIndex); }
    void CopyTo(HDC hdcDest, POINT topLeft);

    float GetDpiScale() const noexcept { return m_dpiScale; }
    void SetDpiScale(float dpiScale) noexcept { m_dpiScale = dpiScale; }

    IDWriteBitmapRenderTarget1* GetRenderTarget() const noexcept { return m_renderTarget.get(); }

    IDWriteTextAnalyzer2* GetTextAnalyzer();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(
        _In_opt_ void* clientDrawingContext,
        _Out_ BOOL* isDisabled
    ) noexcept override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE GetCurrentTransform(
        _In_opt_ void* clientDrawingContext,
        _Out_ DWRITE_MATRIX* transform
    ) noexcept override;

    // IDWritePixelSnapping method
    HRESULT STDMETHODCALLTYPE GetPixelsPerDip(
        _In_opt_ void* clientDrawingContext,
        _Out_ FLOAT* pixelsPerDip
    ) noexcept override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawGlyphRun(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        _In_ DWRITE_GLYPH_RUN const* glyphRun,
        _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer1 method
    HRESULT STDMETHODCALLTYPE DrawGlyphRun(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
        DWRITE_MEASURING_MODE measuringMode,
        _In_ DWRITE_GLYPH_RUN const* glyphRun,
        _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawUnderline(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_UNDERLINE const* underline,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer1 method
    HRESULT STDMETHODCALLTYPE DrawUnderline(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
        _In_ DWRITE_UNDERLINE const* underline,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawStrikethrough(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_STRIKETHROUGH const* strikethrough,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer1 method
    HRESULT STDMETHODCALLTYPE DrawStrikethrough(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
        _In_ DWRITE_STRIKETHROUGH const* strikethrough,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer method
    HRESULT STDMETHODCALLTYPE DrawInlineObject(
        _In_opt_ void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        _In_ IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

    // IDWriteTextRenderer1 method
    HRESULT STDMETHODCALLTYPE DrawInlineObject(
        _In_opt_ void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
        _In_ IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        _In_opt_ IUnknown* clientDrawingEffect
    ) noexcept override;

private:
    static DWRITE_MATRIX CombineTransform(DWRITE_MATRIX a, DWRITE_MATRIX b) noexcept;

    class OrientationTransform
    {
    public:
        OrientationTransform(TextRenderer* renderer,
            DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
            bool isSideways,
            D2D_POINT_2F origin
            );

        ~OrientationTransform();

        // Disallow copying.
        OrientationTransform(const OrientationTransform&) = delete;
        void operator=(const OrientationTransform&) = delete;

    private:
        IDWriteBitmapRenderTarget1* m_renderTarget = nullptr;
        DWRITE_MATRIX m_oldTransform;
    };

    uint32_t m_refCount = 0;
    float m_dpiScale = 1.0f;
    wil::com_ptr<IDWriteRenderingParams> m_renderingParams;
    wil::com_ptr<IDWriteBitmapRenderTarget1> m_renderTarget;
    wil::com_ptr<IDWriteTextAnalyzer2> m_textAnalyzer;
    HDC m_targetHdc = nullptr;
    SIZE m_targetPixelSize = {};
    SIZE m_logicalPixelSize = {};
    COLORREF m_textColor = GetSysColor(COLOR_WINDOWTEXT);
    uint32_t m_colorPaletteIndex = 0;
};
