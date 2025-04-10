// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

template<class T>
struct CompositorTypes;

// Dependent type defintions for the lifted compositor.
template<>
struct CompositorTypes<winrt::Compositor>
{
    using CompositionGraphicsDevice = winrt::CompositionGraphicsDevice;
    using ICompositionGraphicsDeviceInterop = winrt::Microsoft::UI::Composition::ICompositionGraphicsDeviceInterop;
    using CompositionDrawingSurface = winrt::CompositionDrawingSurface;
    using ICompositionDrawingSurfaceInterop = winrt::Microsoft::UI::Composition::ICompositionDrawingSurfaceInterop;
    using ContainerVisual = winrt::ContainerVisual;
    using SpriteVisual = winrt::SpriteVisual;
};

// Dependent type defintions for the system compositor.
template<>
struct CompositorTypes<winrt::WUC::Compositor>
{
    using CompositionGraphicsDevice = winrt::WUC::CompositionGraphicsDevice;
    using ICompositionGraphicsDeviceInterop = ABI::Windows::UI::Composition::ICompositionGraphicsDeviceInterop;
    using CompositionDrawingSurface = winrt::WUC::CompositionDrawingSurface;
    using ICompositionDrawingSurfaceInterop = ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop;
    using ContainerVisual = winrt::WUC::ContainerVisual;
    using SpriteVisual = winrt::WUC::SpriteVisual;
};
