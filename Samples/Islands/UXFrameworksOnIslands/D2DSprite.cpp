// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "D2DSprite.h"
#include "Output.h"
#include "TextRenderer.h"

inline winrt::CompositionDrawingSurface CreateCompositionDrawingSurface(winrt::CompositionGraphicsDevice const& device, winrt::Size pixelSize)
{
    return device.CreateDrawingSurface(
        pixelSize,
        winrt::Microsoft::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Microsoft::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
}

inline winrt::WUC::CompositionDrawingSurface CreateCompositionDrawingSurface(winrt::WUC::CompositionGraphicsDevice const& device, winrt::Size pixelSize)
{
    return device.CreateDrawingSurface(
        pixelSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
}

template <class T>
D2DSprite<T>::D2DSprite(Output<T> const& output, ContainerVisual const& containerVisual) :
    OutputResource<T>(output.GetResourceList()),
    m_containerVisual(containerVisual != nullptr ? containerVisual : output.GetCompositor().CreateContainerVisual()),
    m_spriteVisual(output.GetCompositor().CreateSpriteVisual())
{
    IsPixelSnappingEnabled(!output.GetSetting(Setting_DisablePixelSnapping));
    m_containerVisual.Children().InsertAtTop(m_spriteVisual);
}

template<class T>
void D2DSprite<T>::SetBackgroundColor(winrt::Windows::UI::Color const& color)
{
    if (color != m_backgroundColor)
    {
        m_backgroundColor = color;
        InvalidateContent();
    }
}

template<class T>
void D2DSprite<T>::ReleaseDeviceDependentResources(Output<T> const&)
{
    InvalidateSurface();
}

template<class T>
void D2DSprite<T>::EnsureInitialized(Output<T> const& output)
{
    try
    {
        // Recompute the raster transform based on the current layout.
        auto rasterTransform = ComputeRasterTransform(output);
        if (rasterTransform != m_rasterTransform)
        {
            m_rasterTransform = rasterTransform;
            InvalidateContent();
        }

        // Recompute the bounding box in device coordinates.
        // This invalidates the drawing surface if the size changes.
        InitializePixelBounds(output);

        // Create the drawing surface if it doesn't already exist.
        if (m_drawingSurface == nullptr)
        {
            // Create a CompositionDrawingSurface sized to the pixel bounds.
            m_drawingSurface = CreateCompositionDrawingSurface(output.GetCompositionGraphicsDevice(), GetPixelSize());

            // Create a surface brush and assign it to the visual.
            auto surfaceBrush = output.GetCompositor().CreateSurfaceBrush();
            surfaceBrush.Surface(m_drawingSurface);
            m_spriteVisual.Brush(surfaceBrush);

            m_surfaceGeneration++;
        }

        // Draw the content if it's not already valid.
        if (!m_isContentValid)
        {
            RenderToDrawingSurface(output);
            m_isContentValid = true;
        }

        SetSpriteSizeAndTransform();
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

template<class T>
void D2DSprite<T>::InvalidateSurface()
{
    // Indicate that the content needs to be re-rendered.
    m_isContentValid = false;

    // Indicate that the drawing surface and brush need to be re-created.
    m_drawingSurface = nullptr;
    m_spriteVisual.Brush(nullptr);
}

template<class T>
void D2DSprite<T>::InvalidateContent()
{
    // Indicate that the content needs to be re-rendered.
    m_isContentValid = false;
}

template<class T>
void D2DSprite<T>::OnSettingChanged(Output<T> const& output, SettingId id)
{
    switch (id)
    {
    case Setting_DisablePixelSnapping:
        IsPixelSnappingEnabled(!output.GetSetting(Setting_DisablePixelSnapping));
        break;

    case Setting_ShowSpriteBounds:
    case Setting_ShowSpriteGeneration:
        InvalidateContent();
        break;

    default:
        InvalidateSurface();
    }

    EnsureInitialized(output);
}

template<class T>
Matrix2x2 D2DSprite<T>::ComputeRasterTransform(Output<T> const& output) const
{
    // Compute the aggregate transform (ignoring translation) from the placement visual
    // to the root coordinate space of the island.
    auto matrix = winrt::Windows::Foundation::Numerics::float4x4::identity();
    for (auto visual = GetVisual(); visual != nullptr; visual = visual.Parent())
    {
        // Compute a local transform from the visual's properties, ignoring translation.
        auto visualTransform = winrt::Windows::Foundation::Numerics::make_float4x4_scale(
            visual.Scale());
        visualTransform *= winrt::Windows::Foundation::Numerics::make_float4x4_from_axis_angle(
            visual.RotationAxis(),
            visual.RotationAngle());
        visualTransform *= visual.TransformMatrix();

        // Multiply by the aggregate transform so far.
        matrix *= visualTransform;
    }

    // Flatten to a 2x2 matrix (2D without displacement).
    auto result = Matrix2x2(matrix);

    // Multiply by the island's rasterization transform.
    result = result * output.GetRasterizationTransform();

    // If it's a degenerate transform, use identity.
    if (!result.IsInvertible())
    {
        result = Matrix2x2();
    }

    return result;
}

template <class T>
D2D1_RECT_F D2DSprite<T>::GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform)
{
    // Create a command list and set it as the device context target.
    ID2D1DeviceContext5* deviceContext = output.GetDXDevice().GetDeviceContext().get();
    winrt::com_ptr<ID2D1CommandList> commandList;
    winrt::check_hresult(deviceContext->CreateCommandList(commandList.put()));
    deviceContext->SetTarget(commandList.get());

    // Set the unit mode to pixels because the transform includes DPI scaling.
    deviceContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

    // Render the content with the specified transform, then restore the identity transform.
    deviceContext->BeginDraw();
    deviceContext->SetTransform(rasterTransform);
    RenderContent(output, deviceContext);
    deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    winrt::check_hresult(deviceContext->EndDraw());

    // Restore the original target.
    deviceContext->SetTarget(nullptr);

    // Close the command list and compute its bounds.
    winrt::check_hresult(commandList->Close());
    D2D1_RECT_F worldBounds;
    winrt::check_hresult(deviceContext->GetImageWorldBounds(commandList.get(), /*out*/ &worldBounds));

    return worldBounds;
}

template<class T>
void D2DSprite<T>::InitializePixelBounds(Output<T> const& output)
{
    auto oldPixelSize = GetPixelSize();

    // Get the bounding box of the content in device units, taking into
    // account the raster transform.
    auto transform = m_rasterTransform.ToD2D();
    m_pixelBounds = GetPixelBounds(output, transform);

    // Round to pixel boundaries.
    m_pixelBounds.left = floorf(m_pixelBounds.left);
    m_pixelBounds.top = floorf(m_pixelBounds.top);
    m_pixelBounds.right = ceilf(m_pixelBounds.right);
    m_pixelBounds.bottom = ceilf(m_pixelBounds.bottom);

    // Ensure we recreate the drawing surface and brush if the size changes.
    if (GetPixelSize() != oldPixelSize)
    {
        InvalidateSurface();
    }
}

template<class T>
void D2DSprite<T>::SetSpriteSizeAndTransform()
{
    // Get the render transform, and translate it such that the top-left corner of
    // the pixel bounds is at (0, 0).
    auto transform = m_rasterTransform.ToD2D();
    transform.dx = -m_pixelBounds.left;
    transform.dy = -m_pixelBounds.top;

    // Set the sprite visual's transform to the inverse of the rendering transform.
    // We already applied the transform when rendering the content, so the sprite visual
    // itself should align with the pixel grid.
    //
    // For example, suppose the container visual is rotated clockwise. We apply the inverse
    // transform to the sprite visual to cancel out the rotation. However, the content
    // rendered to the drawing surface is rotated to match the container visual.
    //
    //         /   <-------------------- The container visual is rotated
    //        /
    //       +-----------------+  <----- The sprite visual is axis aligned
    //      /|*******          |
    //     / |     *******     |  <----- The rendered content is rotated
    //    /  |          *******|
    //   /   +-----------------+
    //  /
    transform.Invert();
    auto inverseTransform = winrt::Windows::Foundation::Numerics::float4x4(
        reinterpret_cast<winrt::Windows::Foundation::Numerics::float3x2 const&>(transform));
    m_spriteVisual.TransformMatrix(inverseTransform);

    // The inverse transform cancelled out any scaling (including DPI scale), so the
    // sprite visual's size is measured in pixels. Set the sprite's size to match the
    // bitmap size, so there's no bitmap scaling.
    m_spriteVisual.Size(GetPixelSize());
}

template<class T>
void D2DSprite<T>::RenderToDrawingSurface(Output<T> const& output)
{
    auto drawingSurfaceInterop = m_drawingSurface.as<ICompositionDrawingSurfaceInterop>();

    // Get a device context that's bound to the drawing surface.
    winrt::com_ptr<ID2D1DeviceContext> deviceContext;
    POINT pixelOffset;
    winrt::check_hresult(drawingSurfaceInterop->BeginDraw(
        nullptr,
        __uuidof(decltype(*deviceContext)),
        /*out*/ deviceContext.put_void(),
        /*out*/ &pixelOffset));

    // BeginDraw allocated a rectangle for this drawing surface in an atlas texture.
    // Clip to the bounds of this rectangle.
    const D2D_POINT_2F atlasOffset{ static_cast<float>(pixelOffset.x), static_cast<float>(pixelOffset.y) };
    deviceContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
    deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(atlasOffset.x, atlasOffset.y));
    auto pixelSize = GetPixelSize();
    deviceContext->PushAxisAlignedClip({ 0, 0, pixelSize.Width, pixelSize.Height }, D2D1_ANTIALIAS_MODE_ALIASED);

    // Clear the background.
    deviceContext->Clear(ToColorF(m_backgroundColor));

    // Render using the rasterizer transform, but translated such that the top-left
    // corner of the pixel bounds aligns with the atlasOffset.
    auto transform = m_rasterTransform.ToD2D();
    transform.dx = atlasOffset.x - m_pixelBounds.left;
    transform.dy = atlasOffset.y - m_pixelBounds.top;
    deviceContext->SetTransform(transform);

    // Render the content to the surface.
    RenderContent(output, deviceContext.as<ID2D1DeviceContext5>().get());

    // if "Show Sprite Bounds" is enabled, draw a red rectangle around the pixel bounds.
    if (output.GetSetting(Setting_ShowSpriteBounds))
    {
        deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(atlasOffset.x, atlasOffset.y));
        winrt::com_ptr<ID2D1SolidColorBrush> boundsBrush;
        deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), boundsBrush.put());
        deviceContext->DrawRectangle({ 0, 0, pixelSize.Width, pixelSize.Height }, boundsBrush.get(), m_rasterTransform.ToScaleFactor());
    }

    // If "Show Sprite Generation" is enabled, draw the generation number.
    ++m_renderGeneration;
    if (output.GetSetting(Setting_ShowSpriteGeneration))
    {
        static DigitRenderer renderer(/*fontSize*/ 10.0f);

        // Set a scale transform with (0,0) at the top-left of the drawing surface.
        float scaleFactor = m_rasterTransform.ToScaleFactor();
        deviceContext->SetTransform(
            D2D1::Matrix3x2F::Scale(scaleFactor, scaleFactor) *
            D2D1::Matrix3x2F::Translation(atlasOffset.x, atlasOffset.y)
            );

        // Determine the bottom-right coordinate in logical units.
        auto pixelSize2 = GetPixelSize();
        D2D_POINT_2F bottomRight{pixelSize2.Width / scaleFactor, pixelSize2.Height / scaleFactor};

        auto Draw = [&](uint32_t value, D2D_COLOR_F const& color)
        {
            // Determine the bounds of the number, including padding.
            auto digits = renderer.GetDigitGlyphs(value);

            constexpr float padding = 1;
            D2D_RECT_F numberRect{
                bottomRight.x - renderer.Width(digits) - (padding * 2),
                bottomRight.y - renderer.Height() - (padding * 2),
                bottomRight.x,
                bottomRight.y
            };

            // The next number will be drawn to the left of this one.
            bottomRight.x = numberRect.left;

            // Determine the top-left corner of the digits.
            D2D_POINT_2F numberPos{numberRect.left + padding, numberRect.top + padding};

            // Fill the background.
            winrt::com_ptr<ID2D1SolidColorBrush> brush;
            deviceContext->CreateSolidColorBrush(color, brush.put());
            deviceContext->FillRectangle(numberRect, brush.get());

            // Draw the number.
            brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
            renderer.Draw(deviceContext.get(), numberPos, digits, brush.get());
        };

        Draw(m_renderGeneration, D2D1::ColorF(D2D1::ColorF::DarkRed));
        Draw(m_surfaceGeneration, D2D1::ColorF(D2D1::ColorF::Blue));
    }

    // Reset the transform to identity and end drawing.
    deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
}

template class D2DSprite<winrt::Compositor>;
template class D2DSprite<winrt::WUC::Compositor>;
