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
    using ContainerVisual = typename CompositorTypes<T>::ContainerVisual;
    using SpriteVisual = typename CompositorTypes<T>::SpriteVisual;

    // Constructs a D2DSprite object with its own container visual.
    D2DSprite(Output<T> const& output);

    // Constructs a D2DSPrite object with a specified container visual.
    // Use this constructor if multiple sprites share the same container.
    D2DSprite(Output<T> const& output, ContainerVisual const& containerVisual);

    // OutputResource mthods.
    void ReleaseDeviceDependentResources(Output<T> const& output) override;
    void CreateDeviceDependentResources(Output<T> const& output) override;
    void HandleRasterizationScaleChanged(Output<T> const& output) override;

    // Initialize the sprite visual to point to a surface brush with newly-rendered content.
    void InitializeSpriteVisual(Output<T> const& output);

    // Gets the container visual, which is used for layout. The sprite visual is
    // positioned at an offset from this.
    auto& GetVisual() const noexcept
    {
        return m_containerVisual;
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

protected:

    // Gets the bounding box of the content in pixels relative to origin of the container visual.
    // The default implementation computes the bounding box by rendering the content to a command
    // list. A derived implementation may override this method if the bounds are easily computed.
    virtual D2D1_RECT_F GetPixelBounds(Output<T> const& output, D2D1::Matrix3x2F const& rasterTransform);

    // A derived class must implement this method to render the content.
    virtual void RenderContent(ID2D1DeviceContext5* deviceContext) = 0;

private:
    Matrix2x2 ComputeRasterTransform(Output<T> const& output) const;

    ContainerVisual m_containerVisual = nullptr;
    SpriteVisual m_spriteVisual = nullptr;

    Matrix2x2 m_rasterTransform;
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
