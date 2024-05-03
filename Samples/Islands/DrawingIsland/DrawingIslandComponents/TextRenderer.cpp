// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextRenderer::TextRenderer()
    {
        // Create the DWriteCore factory object.
        THROW_IF_FAILED(DWriteCoreCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(decltype(*m_dwriteFactory)),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.put())
        ));

        // Create an object that encapsulates text formatting properties.
        constexpr float defaultFontSize = 16;
        THROW_IF_FAILED(m_dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            defaultFontSize,
            L"en-US",
            m_textFormat.put()
        ));
        THROW_IF_FAILED(m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        // The bitmap render target is created through the IDWriteGdiInterop
        // interface.
        winrt::com_ptr<IDWriteGdiInterop> gdiInterop;
        THROW_IF_FAILED(m_dwriteFactory->GetGdiInterop(gdiInterop.put()));

        // Create the bitmap render target and get a pointer to the bitmap data.
        winrt::com_ptr<IDWriteBitmapRenderTarget> renderTarget;
        THROW_IF_FAILED(gdiInterop->CreateBitmapRenderTarget(nullptr, 1, 1, renderTarget.put()));

        m_renderTarget = renderTarget.as<IDWriteBitmapRenderTarget3>();
        m_renderTarget->GetBitmapData(/*out*/ &m_bitmapData);

        // Create the rendering params object.
        m_dwriteFactory->CreateRenderingParams(m_renderingParams.put());
    }

    void TextRenderer::Render(_In_z_ WCHAR const* text)
    {
        winrt::com_ptr<IDWriteTextLayout> textLayout;
        THROW_IF_FAILED(m_dwriteFactory->CreateTextLayout(
            text,
            static_cast<uint32_t>(wcslen(text)),
            m_textFormat.get(),
            /*maxWidth*/ 0,
            /*maxHeight*/ 0,
            /*out*/ textLayout.put()
        ));

        DWRITE_TEXT_METRICS textMetrics;
        THROW_IF_FAILED(textLayout->GetMetrics(/*out*/ &textMetrics));

        Resize(textMetrics.width, textMetrics.height);

        ClearBackground();

        THROW_IF_FAILED(textLayout->Draw(nullptr, this, 0, 0));
    }

    void TextRenderer::Resize(float width, float height)
    {
        uint32_t pixelWidth = static_cast<uint32_t>(ceilf(width * m_dpiScale));
        uint32_t pixelHeight = static_cast<uint32_t>(ceilf(height * m_dpiScale));

        THROW_IF_FAILED(m_renderTarget->Resize(pixelWidth, pixelHeight));

        m_renderTarget->GetBitmapData(/*out*/ &m_bitmapData);
        m_size = { width, height };
    }

    void TextRenderer::ClearBackground() noexcept
    {
        std::fill_n(m_bitmapData.pixels, m_bitmapData.width * m_bitmapData.height, m_backgroundColor);
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
}
