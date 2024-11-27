// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "D2DSprite.h"
#include "Output.h"

#define SHOW_SPRITE_BOUNDS 0

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
D2DSprite<T>::D2DSprite(Output<T> const& output) :
    D2DSprite<T>(output, output.GetCompositor().CreateContainerVisual())
{
}

template <class T>
D2DSprite<T>::D2DSprite(Output<T> const& output, ContainerVisual const& containerVisual) :
    OutputResource<T>(output.GetResourceList()),
    m_containerVisual(containerVisual),
    m_spriteVisual(output.GetCompositor().CreateSpriteVisual())
{
    m_spriteVisual.IsPixelSnappingEnabled(true);
    containerVisual.Children().InsertAtTop(m_spriteVisual);
}

template<class T>
void D2DSprite<T>::ReleaseDeviceDependentResources(Output<T> const&)
{
    // Discard the surface brush.
    m_spriteVisual.Brush(nullptr);
}

template<class T>
void D2DSprite<T>::CreateDeviceDependentResources(Output<T> const& output)
{
    // Initialize the visual's brush if we haven't already.
    if (m_spriteVisual.Brush() == nullptr)
    {
        InitializeSpriteVisual(output);
    }
}

template<class T>
void D2DSprite<T>::HandleRasterizationScaleChanged(Output<T> const& output)
{
    auto newTransform = ComputeRasterTransform(output);

    // Do nothing if the transform has not changed or is not invertible.
    if (newTransform == m_rasterTransform || !newTransform.IsInvertible())
    {
        return;
    }

    // Save the new transform.
    m_rasterTransform = newTransform;

    // Discard the old surface brush.
    m_spriteVisual.Brush(nullptr);

    // Create a new surface brush with the correct scale.
    InitializeSpriteVisual(output);
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

    // Multiply by the rasterizatoin scale for the island.
    matrix *= winrt::Windows::Foundation::Numerics::make_float4x4_scale(
            output.GetRasterizationScale());

    // Flatten to a 2x2 matrix, since that's all we care about for rasterization.
    return Matrix2x2(matrix);
}

template<class T>
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
    RenderContent(deviceContext);
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

// Initialize the sprite visual to point to a surface brush with newly-rendered content.
template<class T>
void D2DSprite<T>::InitializeSpriteVisual(Output<T> const& output)
{
    try
    {
        auto transform = m_rasterTransform.ToD2D();

        // Get the bounding box of the content in device units.
        D2D1_RECT_F pixelBounds = GetPixelBounds(output, transform);

        // Round to pixel boundaries.
        pixelBounds.left = floorf(pixelBounds.left);
        pixelBounds.top = floorf(pixelBounds.top);
        pixelBounds.right = ceilf(pixelBounds.right);
        pixelBounds.bottom = ceilf(pixelBounds.bottom);

        // Create a CompositionDrawingSurface sized to the pixelBounds.
        const winrt::Size pixelSize(pixelBounds.right - pixelBounds.left, pixelBounds.bottom - pixelBounds.top);
        auto drawingSurface = CreateCompositionDrawingSurface(output.GetCompositionGraphicsDevice(), pixelSize);
        auto drawingSurfaceInterop = drawingSurface.as<ICompositionDrawingSurfaceInterop>();

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
        deviceContext->PushAxisAlignedClip({ 0, 0, pixelSize.Width, pixelSize.Height }, D2D1_ANTIALIAS_MODE_ALIASED);

        // Clear to transparent.
        deviceContext->Clear(D2D_COLOR_F{});

        // Render using the rasterizer transform, but translated such that the top-left
        // corner of the pixelBounds aligns with the atlasOffset.
        transform.dx = atlasOffset.x - pixelBounds.left;
        transform.dy = atlasOffset.y - pixelBounds.top;
        deviceContext->SetTransform(transform);

        // Render the content to the surface.
        RenderContent(deviceContext.as<ID2D1DeviceContext5>().get());

#if SHOW_SPRITE_BOUNDS
        // Draw a white rectangle around the sprite bounds for debugging purposes.
        deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(atlasOffset.x, atlasOffset.y));
        winrt::com_ptr<ID2D1SolidColorBrush> boundsBrush;
        deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), boundsBrush.put());
        deviceContext->DrawRectangle({ 0, 0, pixelSize.Width, pixelSize.Height }, boundsBrush.get());
#endif

        // Reset the transform to identity and end drawing.
        deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
        winrt::check_hresult(drawingSurfaceInterop->EndDraw());

        // Create a surface brush and assign it to the visual.
        auto surfaceBrush = output.GetCompositor().CreateSurfaceBrush();
        surfaceBrush.Surface(drawingSurface);
        m_spriteVisual.Brush(surfaceBrush);

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
        transform.dx -= atlasOffset.x;
        transform.dy -= atlasOffset.y;
        transform.Invert();

        auto inverseTransform = winrt::Windows::Foundation::Numerics::float4x4(
            reinterpret_cast<winrt::Windows::Foundation::Numerics::float3x2 const&>(transform));
        m_spriteVisual.TransformMatrix(inverseTransform);

        // The inverse transform cancelled out any scaling (including DPI scale), so the
        // sprite visual's size is measured in pixels. Set the sprite's size to match the
        // bitmap size, so there's no bitmap scaling.
        m_spriteVisual.Size(pixelSize);
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

template class D2DSprite<winrt::Compositor>;
template class D2DSprite<winrt::WUC::Compositor>;
