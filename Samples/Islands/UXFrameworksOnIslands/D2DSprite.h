// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "OutputResource.h"
#include "TemplateHelpers.h"
#include "Matrix2x2.h"

// Base class for objects that render to a SpriteVisual using Direct2D.
// This class derives from OutputResource so the content can be re-rendered
// if the rasterization scale changes or the device is lost.
template<class T>
class D2DSprite : public OutputResource<T>
{
public:
    using ICompositionDrawingSurfaceInterop = typename CompositorTypes<T>::ICompositionDrawingSurfaceInterop;
    using CompositionDrawingSurface = typename CompositorTypes<T>::CompositionDrawingSurface;
    using ContainerVisual = typename CompositorTypes<T>::ContainerVisual;
    using SpriteVisual = typename CompositorTypes<T>::SpriteVisual;

    // Constructs a D2DSPrite object. A container visual may be specified if the client
    // wants multiple sprites to share the same container.
    D2DSprite(Output<T> const& output, ContainerVisual const& containerVisual = nullptr);

    // OutputResource methods.
    void ReleaseDeviceDependentResources(Output<T> const& output) override;
    void EnsureInitialized(Output<T> const& output) override;
    void OnSettingChanged(Output<T> const& output, SettingId id) override;

    void InvalidateContent();
    
    // Gets the container visual, which is used for layout. The sprite visual is
    // positioned at an offset from this.
    auto& GetVisual() const noexcept
    {
        return m_containerVisual;
    }

    // Getter and setter for the IsVisible property of the underlying sprite visual.
    bool IsVisible() const
    {
        return m_spriteVisual.IsVisible();
    }

    void IsVisible(bool value)
    {
        m_spriteVisual.IsVisible(value);
    }

    // Getter and setter for the the IsPixelSnappingEnabled property of the underlying sprite visual.
    // This is true by default to avoid undesirable bitmap interpolation after the content is rendered.
    bool IsPixelSnappingEnabled() const
    {
        return m_spriteVisual.IsPixelSnappingEnabled();
    }

    void IsPixelSnappingEnabled(bool value)
    {
        m_spriteVisual.IsPixelSnappingEnabled(value);
    }

    winrt::Windows::UI::Color const& GetBackgroundColor() const noexcept
    {
        return m_backgroundColor;
    }

    void SetBackgroundColor(winrt::Windows::UI::Color const& color);

protected:

    // Gets the bounding box of the content in pixels relative to origin of the container visual.
    // The default implementation computes the bounding box by rendering the content to a command
    // list. A derived implementation may override this method if the bounds are easily computed.
    virtual D2D1_RECT_F GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform);

    // A derived class must implement this method to render the content.
    virtual void RenderContent(Output<T> const& output, ID2D1DeviceContext5* deviceContext) = 0;

private:
    winrt::Size GetPixelSize() const noexcept
    {
        return { m_pixelBounds.right - m_pixelBounds.left, m_pixelBounds.bottom - m_pixelBounds.top };
    }

    void InvalidateSurface();
    void InitializePixelBounds(Output<T> const& output);
    void SetSpriteSizeAndTransform();
    void RenderToDrawingSurface(Output<T> const& output);
    Matrix2x2 ComputeRasterTransform(Output<T> const& output) const;

    winrt::Windows::UI::Color m_backgroundColor = winrt::Windows::UI::Colors::Transparent();

    ContainerVisual m_containerVisual = nullptr;
    SpriteVisual m_spriteVisual = nullptr;
    CompositionDrawingSurface m_drawingSurface{ nullptr };
    bool m_isContentValid = false;
    uint32_t m_surfaceGeneration = 0;
    uint32_t m_renderGeneration = 0;

    Matrix2x2 m_rasterTransform;
    D2D1_RECT_F m_pixelBounds = {};
};

using LiftedD2DSprite = D2DSprite<winrt::Compositor>;
using SystemD2DSprite = D2DSprite<winrt::WUC::Compositor>;

// Trivial conversion helpers.
inline D2D_COLOR_F ToColorF(winrt::Windows::UI::Color const& color) noexcept
{
    return { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
}

inline D2D_SIZE_F ToSizeF(winrt::Size const& size) noexcept
{
    return { static_cast<float>(size.Width), static_cast<float>(size.Height) };
}
