// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TextRenderer.h"

namespace winrt::DrawingIslandComponents::implementation
{
    TextRenderer::TextRenderer(winrt::Compositor compositor) : m_compositor(compositor)
    {
        // Create the DWrite factory object.
        winrt::check_hresult(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(decltype(*m_dwriteFactory)),
            reinterpret_cast<::IUnknown**>(m_dwriteFactory.put())));

        // Create an object that encapsulates text formatting properties.
        constexpr float defaultFontSize = 16;
        winrt::check_hresult(m_dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            defaultFontSize,
            L"en-US",
            m_textFormat.put()));
        winrt::check_hresult(m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        // Create the D2D factory.
        winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_d2dFactory.put()));

        // Create the Direct3D and Direct2D device objects.
        CreateDirect2DDevice();

        // Create the composition graphics device.
        auto compositorInterop = m_compositor.as<ICompositorInterop>();
        ICompositionGraphicsDevice compositionGraphicsDevice;
        winrt::check_hresult(compositorInterop->CreateGraphicsDevice(m_d2dDevice.get(), &compositionGraphicsDevice));
        m_compositionGraphicsDevice = compositionGraphicsDevice.as<CompositionGraphicsDevice>();
    }

    void TextRenderer::RecreateDirect2DDevice()
    {
        // Release the old Direct2D and Direct2D device objects.
        m_dxgiDevice = nullptr;
        m_d2dDevice = nullptr;

        // Create new Direct3D and Direct2D device objects.
        CreateDirect2DDevice();

        // Restore the composition graphics device to health by pointing to the new Direct2D device.
        auto compositionGraphicsDeviceInterop = m_compositionGraphicsDevice.as<ICompositionGraphicsDeviceInterop>();
        winrt::check_hresult(compositionGraphicsDeviceInterop->SetRenderingDevice(m_d2dDevice.get()));
    }

    void TextRenderer::CreateDirect2DDevice()
    {
        uint32_t createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        // Array with DirectX hardware feature levels in order of preference.
        static const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        // Create the Direct3D 11 API device object and a corresponding context.
        winrt::com_ptr<::ID3D11Device> d3dDevice;
        winrt::com_ptr<::ID3D11DeviceContext> d3dImmediateContext;
        D3D_FEATURE_LEVEL d3dFeatureLevel{ D3D_FEATURE_LEVEL_9_1 };

        winrt::check_hresult(D3D11CreateDevice(
            nullptr, // Default adapter.
            D3D_DRIVER_TYPE_HARDWARE,
            0, // Not asking for a software driver, so not passing a module to one.
            createDeviceFlags, // Set debug and Direct2D compatibility flags.
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            d3dDevice.put(),
            &d3dFeatureLevel,
            d3dImmediateContext.put()));

        // Initialize Direct2D resources.
        D2D1_FACTORY_OPTIONS d2d1FactoryOptions{ D2D1_DEBUG_LEVEL_NONE };

        // Create the Direct2D device object.
        // Obtain the underlying DXGI device of the Direct3D device.
        auto dxgiDevice = d3dDevice.as<::IDXGIDevice>();

        winrt::com_ptr<ID2D1Device6> d2dDevice;
        winrt::check_hresult(m_d2dFactory->CreateDevice(dxgiDevice.get(), d2dDevice.put()));

        // Save the newly-created objects.
        m_dxgiDevice = std::move(dxgiDevice);
        m_d2dDevice = std::move(d2dDevice);
    }

    void TextRenderer::Render(
        _In_z_ WCHAR const* text,
        Windows::UI::Color backgroundColor,
        Windows::UI::Color textColor,
        SpriteVisual const& visual)
    {
        // Margins between the layout bounds of the text and the edges of the visual.
        // These are in device-independent pixels (DIPs).
        constexpr float marginLeft = 5;
        constexpr float marginRight = 5;
        constexpr float marginTop = 5;
        constexpr float marginBottom = 5;

        // Create the IDWriteTextlayout object. This will be used to render the text
        // and can also be used to get properties, such as the size.
        winrt::com_ptr<IDWriteTextLayout> textLayout;
        winrt::check_hresult(m_dwriteFactory->CreateTextLayout(
            text,
            static_cast<uint32_t>(wcslen(text)),
            m_textFormat.get(),
            /*maxWidth*/ 0,
            /*maxHeight*/ 0,
            /*out*/ textLayout.put()));

        // Get the metrics from the text layout, and add the margins to compute the
        // width and height of the visual.
        DWRITE_TEXT_METRICS textMetrics;
        winrt::check_hresult(textLayout->GetMetrics(/*out*/ &textMetrics));
        const float width = textMetrics.width + (marginLeft + marginRight);
        const float height = textMetrics.height + (marginTop + marginBottom);

        visual.Size(float2(width, height));

        try
        {
            // Create a composition surface to draw to.
            CompositionDrawingSurface drawingSurface = m_compositionGraphicsDevice.CreateDrawingSurface(
                winrt::Windows::Foundation::Size(width * m_dpiScale, height * m_dpiScale),
                winrt::Microsoft::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                winrt::Microsoft::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
            auto drawingSurfaceInterop = drawingSurface.as<ICompositionDrawingSurfaceInterop>();

            // Begin drawing to get a Direct2D device context.
            winrt::com_ptr<ID2D1DeviceContext> deviceContext;
            POINT pixelOffset;
            winrt::check_hresult(drawingSurfaceInterop->BeginDraw(
                nullptr,
                __uuidof(ID2D1DeviceContext),
                deviceContext.put_void(),
                &pixelOffset));

            // Set the DPI of the device context, where 96 DPI corresponds to a 1.0 scale factor.
            deviceContext->SetDpi(m_dpiScale * 96, m_dpiScale * 96);

            // Compute the origin (top-left corner) of the text layout in DIPs by converting
            // the drawing surface offset from pixels to DIPs and adding the margin.
            D2D_POINT_2F origin{
                pixelOffset.x / m_dpiScale + marginLeft,
                pixelOffset.y / m_dpiScale + marginTop };

            // Clear the background and draw the text.
            deviceContext->Clear(ToColorF(backgroundColor));

            // Use ClearType antialiasing if rendering onto an opaque background.
            // Otherwise use grayscale.
            deviceContext->SetTextAntialiasMode(
                backgroundColor.A == 255 ?
                D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE :
                D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

            // Create the brush used to fill the text.
            winrt::com_ptr<ID2D1SolidColorBrush> textBrush;
            winrt::check_hresult(deviceContext->CreateSolidColorBrush(ToColorF(textColor), textBrush.put()));

            // Draw the text layout object.
            deviceContext->DrawTextLayout(origin, textLayout.get(), textBrush.get());

            // End drawing.
            winrt::check_hresult(drawingSurfaceInterop->EndDraw());

            // Create the surface brush and set it as the visual's brush.
            auto surfaceBrush = m_compositor.CreateSurfaceBrush();
            surfaceBrush.Surface(drawingSurface);
            visual.Brush(surfaceBrush);

            surfaceBrush.SnapToPixels(true);
            visual.IsPixelSnappingEnabled(true);
        }
        catch (winrt::hresult_error& e)
        {
            // Silently ignore device-removed error.
            // We'll draw again after the device is recreated.
            if (e.code() == DXGI_ERROR_DEVICE_REMOVED)
                return;

            // Rethrow all other exceptions.
            throw;
        }
    }
}
