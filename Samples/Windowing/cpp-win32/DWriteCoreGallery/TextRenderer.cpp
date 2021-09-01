// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer(HWND hwnd)
{
    auto hdc = wil::GetDC(hwnd);
    THROW_LAST_ERROR_IF_NULL(hdc);

    int dpi = GetDeviceCaps(hdc.get(), LOGPIXELSY);
    m_dpiScale = dpi * (1.0f / 96);

    THROW_IF_FAILED(g_factory->CreateRenderingParams(&m_renderingParams));

    wil::com_ptr<IDWriteGdiInterop> gdiInterop;
    THROW_IF_FAILED(g_factory->GetGdiInterop(&gdiInterop));

    constexpr SIZE initialSize = { 1024, 256 };
    wil::com_ptr<IDWriteBitmapRenderTarget> renderTarget;
    THROW_IF_FAILED(gdiInterop->CreateBitmapRenderTarget(hdc.get(), initialSize.cx, initialSize.cy, &renderTarget));

    renderTarget.query_to(&m_renderTarget);
    m_targetHdc = renderTarget->GetMemoryDC();
    m_targetPixelSize = initialSize;
}

IDWriteTextAnalyzer2* TextRenderer::GetTextAnalyzer()
{
    if (m_textAnalyzer == nullptr)
    {
        wil::com_ptr<IDWriteTextAnalyzer> textAnalyzer;
        THROW_IF_FAILED(g_factory->CreateTextAnalyzer(&textAnalyzer));
        textAnalyzer.query_to(&m_textAnalyzer);
    }
    return m_textAnalyzer.get();
}

void TextRenderer::Resize(SIZE pixelSize)
{
    if (pixelSize.cx > m_targetPixelSize.cx || pixelSize.cy > m_targetPixelSize.cy)
    {
        int width = std::max(pixelSize.cx, m_targetPixelSize.cx);
        int height = std::max(pixelSize.cy, m_targetPixelSize.cy);

        THROW_IF_FAILED(m_renderTarget->Resize(width, height));

        m_targetPixelSize = { width, height };
    }

    m_logicalPixelSize = pixelSize;
}

void TextRenderer::Clear(int sysColorIndex)
{
    RECT rect = { 0, 0, m_logicalPixelSize.cx, m_logicalPixelSize.cy };
    FillRect(m_targetHdc, &rect, GetSysColorBrush(sysColorIndex));
}

void TextRenderer::CopyTo(HDC hdcDest, POINT topLeft)
{
    BitBlt(hdcDest, topLeft.x, topLeft.y, m_logicalPixelSize.cx, m_logicalPixelSize.cy, m_targetHdc, 0, 0, SRCCOPY);
}

// IUnknown method
HRESULT STDMETHODCALLTYPE TextRenderer::QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject) noexcept
{
    if (riid == __uuidof(IDWriteTextRenderer1) ||
        riid == __uuidof(IDWriteTextRenderer) ||
        riid == __uuidof(IDWritePixelSnapping) ||
        riid == __uuidof(IUnknown))
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
}

// IUnknown method
ULONG STDMETHODCALLTYPE TextRenderer::AddRef() noexcept
{
    return ++m_refCount;
}

// IUnknown method
ULONG STDMETHODCALLTYPE TextRenderer::Release() noexcept
{
    uint32_t newCount = --m_refCount;
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}

DWRITE_MATRIX TextRenderer::CombineTransform(DWRITE_MATRIX a, DWRITE_MATRIX b) noexcept
{
    return
    {
        a.m11 * b.m11 + a.m12 * b.m21,
        a.m11 * b.m12 + a.m12 * b.m22,
        a.m21 * b.m11 + a.m22 * b.m21,
        a.m21 * b.m12 + a.m22 * b.m22,
        a.dx * b.m11 + a.dy * b.m21 + b.dx,
        a.dx * b.m12 + a.dy * b.m22 + b.dy
    };
}

TextRenderer::OrientationTransform::OrientationTransform(TextRenderer* renderer,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    bool isSideways,
    D2D_POINT_2F origin
)
{
    if (orientationAngle != DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES)
    {
        m_renderTarget = renderer->GetRenderTarget();
        THROW_IF_FAILED(m_renderTarget->GetCurrentTransform(&m_oldTransform));

        DWRITE_MATRIX orientationTransform;
        THROW_IF_FAILED(renderer->GetTextAnalyzer()->GetGlyphOrientationTransform(orientationAngle, isSideways, origin.x, origin.y, &orientationTransform));

        DWRITE_MATRIX finalTransform = CombineTransform(m_oldTransform, orientationTransform);
        THROW_IF_FAILED(m_renderTarget->SetCurrentTransform(&finalTransform));
    }
}

TextRenderer::OrientationTransform::~OrientationTransform()
{
    if (m_renderTarget != nullptr)
    {
        m_renderTarget->SetCurrentTransform(&m_oldTransform);
    }
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::IsPixelSnappingDisabled(
    _In_opt_ void* clientDrawingContext,
    _Out_ BOOL* isDisabled
    ) noexcept
{
    *isDisabled = false;
    return S_OK;
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::GetCurrentTransform(
    _In_opt_ void* clientDrawingContext,
    _Out_ DWRITE_MATRIX* transform
    ) noexcept
{
    *transform = DWRITE_MATRIX{ 1, 0, 0, 1, 0, 0 };
    return S_OK;
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::GetPixelsPerDip(
    _In_opt_ void* clientDrawingContext,
    _Out_ FLOAT* pixelsPerDip
    ) noexcept
{
    *pixelsPerDip = m_dpiScale;
    return S_OK;
}

inline BYTE FloatToColorByte(float c)
{
    return static_cast<BYTE>(floorf(c * 255 + 0.5f));
}

COLORREF ToCOLORREF(DWRITE_COLOR_F color)
{
    return RGB(
        FloatToColorByte(color.r),
        FloatToColorByte(color.g),
        FloatToColorByte(color.b)
    );
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawGlyphRun(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    _In_ DWRITE_GLYPH_RUN const* glyphRun,
    _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    return DrawGlyphRun(
        clientDrawingContext,
        baselineOriginX,
        baselineOriginY,
        DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
        measuringMode,
        glyphRun,
        glyphRunDescription,
        clientDrawingEffect
    );
}

// IDWriteTextRenderer1 method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawGlyphRun(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    DWRITE_MEASURING_MODE measuringMode,
    _In_ DWRITE_GLYPH_RUN const* glyphRun,
    _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    try
    {
        OrientationTransform orentation(
            this,
            orientationAngle,
            !!glyphRun->isSideways,
            D2D_POINT_2F{ baselineOriginX, baselineOriginY }
        );

        HRESULT hr = DWRITE_E_NOCOLOR;
        wil::com_ptr<IDWriteColorGlyphRunEnumerator> colorLayers;

        wil::com_ptr<IDWriteFontFace2> fontFace2;
        if (SUCCEEDED(glyphRun->fontFace->QueryInterface(&fontFace2)))
        {
            uint32_t paletteCount = fontFace2->GetColorPaletteCount();
            if (paletteCount > 0)
            {
                DWRITE_MATRIX transform;
                hr = m_renderTarget->GetCurrentTransform(&transform);
                if (FAILED(hr))
                {
                    return hr;
                }

                transform.m11 *= m_dpiScale;
                transform.m12 *= m_dpiScale;
                transform.m21 *= m_dpiScale;
                transform.m22 *= m_dpiScale;
                transform.dx *= m_dpiScale;
                transform.dy *= m_dpiScale;

                // Perform color translation.
                // Fall back to the default palette if the current palette index is out of range.
                hr = g_factory->TranslateColorGlyphRun(
                    baselineOriginX,
                    baselineOriginY,
                    glyphRun,
                    nullptr,
                    measuringMode,
                    &transform,
                    m_colorPaletteIndex < paletteCount ? m_colorPaletteIndex : 0,
                    & colorLayers
                );
            }
        }

        if (hr == DWRITE_E_NOCOLOR)
        {
            THROW_IF_FAILED(m_renderTarget->DrawGlyphRun(
                baselineOriginX,
                baselineOriginY,
                measuringMode,
                glyphRun,
                m_renderingParams.get(),
                m_textColor
            ));
        }
        else
        {
            THROW_IF_FAILED(hr);

            for (;;)
            {
                BOOL haveRun;
                THROW_IF_FAILED(colorLayers->MoveNext(&haveRun));
                if (!haveRun)
                {
                    break;
                }

                DWRITE_COLOR_GLYPH_RUN const* colorRun;
                THROW_IF_FAILED(colorLayers->GetCurrentRun(&colorRun));

                COLORREF runColor = (colorRun->paletteIndex == 0xFFFF) ? m_textColor : ToCOLORREF(colorRun->runColor);

                THROW_IF_FAILED(m_renderTarget->DrawGlyphRun(
                    colorRun->baselineOriginX,
                    colorRun->baselineOriginY,
                    measuringMode,
                    &colorRun->glyphRun,
                    m_renderingParams.get(),
                    runColor
                ));
            }
        }

        return S_OK;
    }
    catch (wil::ResultException& e)
    {
        return e.GetErrorCode();
    }
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawUnderline(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    _In_ DWRITE_UNDERLINE const* underline,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    // TODO - text decorations
    return E_NOTIMPL;
}

// IDWriteTextRenderer1 method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawUnderline(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    _In_ DWRITE_UNDERLINE const* underline,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    // TODO - text decorations
    return E_NOTIMPL;
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawStrikethrough(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    _In_ DWRITE_STRIKETHROUGH const* strikethrough,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    // TODO - text decorations
    return E_NOTIMPL;
}

// IDWriteTextRenderer1 method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawStrikethrough(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    _In_ DWRITE_STRIKETHROUGH const* strikethrough,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    // TODO - text decorations
    return E_NOTIMPL;
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawInlineObject(
    _In_opt_ void* clientDrawingContext,
    FLOAT originX,
    FLOAT originY,
    _In_ IDWriteInlineObject* inlineObject,
    BOOL isSideways,
    BOOL isRightToLeft,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    return DrawInlineObject(
        clientDrawingContext,
        originX,
        originY,
        DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
        inlineObject,
        isSideways,
        isRightToLeft,
        clientDrawingEffect
    );
}

// IDWriteTextRenderer1 method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawInlineObject(
    _In_opt_ void* clientDrawingContext,
    FLOAT originX,
    FLOAT originY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    _In_ IDWriteInlineObject* inlineObject,
    BOOL isSideways,
    BOOL isRightToLeft,
    _In_opt_ IUnknown* clientDrawingEffect
) noexcept
{
    try
    {
        OrientationTransform orentation(
            this,
            orientationAngle,
            !!isSideways,
            D2D_POINT_2F{ originX, originY }
        );

        THROW_IF_FAILED(inlineObject->Draw(
            clientDrawingContext,
            this,
            originX,
            originY,
            isSideways,
            isRightToLeft,
            clientDrawingEffect
        ));

        return S_OK;
    }
    catch (wil::ResultException& e)
    {
        return e.GetErrorCode();
    }
}

