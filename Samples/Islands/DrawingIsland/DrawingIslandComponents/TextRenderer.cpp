// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer()
{
    THROW_IF_FAILED(DWriteCoreCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(decltype(*m_dwriteFactory)),
        reinterpret_cast<IUnknown**>(m_dwriteFactory.put())
    ));
}

// IUnknown method
HRESULT STDMETHODCALLTYPE TextRenderer::QueryInterface(
    REFIID riid,
    _COM_Outptr_ void** ppvObject
    )
{
    if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWritePixelSnapping) || riid == __uuidof(IDWriteTextRenderer))
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
ULONG STDMETHODCALLTYPE TextRenderer::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

// IUnknown method
ULONG STDMETHODCALLTYPE TextRenderer::Release(void)
{
    ULONG newCount = InterlockedDecrement(&m_refCount);
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::IsPixelSnappingDisabled(
    _In_opt_ void* /*clientDrawingContext*/,
    _Out_ BOOL* isDisabled
)
{
    // Pixel snapping means rounding the baseline coordinate of text to a pixel boundary.
    // This is normally enabled to improve sharpness of text.
    *isDisabled = false;
    return S_OK;
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::GetCurrentTransform(
    _In_opt_ void* /*clientDrawingContext*/,
    _Out_ DWRITE_MATRIX* transform
)
{
    // Identity transform
    *transform = DWRITE_MATRIX{ 1, 0, 0, 1, 0, 0 };
    return S_OK;
}

// IDWritePixelSnapping method
HRESULT STDMETHODCALLTYPE TextRenderer::GetPixelsPerDip(
    _In_opt_ void* /*clientDrawingContext*/,
    _Out_ FLOAT* pixelsPerDip
)
{
    *pixelsPerDip = m_dpiScale;
    return S_OK;
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawGlyphRun(
    _In_opt_ void* /*clientDrawingContext*/,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    _In_ DWRITE_GLYPH_RUN const* glyphRun,
    _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* /*glyphRunDescription*/,
    _In_opt_ IUnknown* /*clientDrawingEffect*/
)
{
    return m_renderTarget->DrawGlyphRunWithColorSupport(
        baselineOriginX,
        baselineOriginY,
        measuringMode,
        glyphRun,
        m_renderingParams.get(),
        m_textColor
    );
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawUnderline(
    _In_opt_ void* /*clientDrawingContext*/,
    FLOAT /*baselineOriginX*/,
    FLOAT /*baselineOriginY*/,
    _In_ DWRITE_UNDERLINE const* /*underline*/,
    _In_opt_ IUnknown* /*clientDrawingEffect*/
)
{
    // noop
    return S_OK;
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawStrikethrough(
    _In_opt_ void* /*clientDrawingContext*/,
    FLOAT /*baselineOriginX*/,
    FLOAT /*baselineOriginY*/,
    _In_ DWRITE_STRIKETHROUGH const* /*strikethrough*/,
    _In_opt_ IUnknown* /*clientDrawingEffect*/
)
{
    // noop
    return S_OK;
}

// IDWriteTextRenderer method
HRESULT STDMETHODCALLTYPE TextRenderer::DrawInlineObject(
    _In_opt_ void* /*clientDrawingContext*/,
    FLOAT /*originX*/,
    FLOAT /*originY*/,
    _In_ IDWriteInlineObject* /*inlineObject*/,
    BOOL /*isSideways*/,
    BOOL /*isRightToLeft*/,
    _In_opt_ IUnknown* /*clientDrawingEffect*/
)
{
    // noop
    return S_OK;
}
