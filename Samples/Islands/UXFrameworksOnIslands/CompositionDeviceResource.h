// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "OutputResource.h"
#include "TemplateHelpers.h"

// Wrapper for CompositionGraphicsDevice.
// This class derives from OutputResource so it can recover from device-lost.
template <class T>
class CompositionDeviceResource final : public OutputResource<T>
{
public:
    using CompositionGraphicsDevice = typename CompositorTypes<T>::CompositionGraphicsDevice;
    using ICompositionGraphicsDeviceInterop = typename CompositorTypes<T>::ICompositionGraphicsDeviceInterop;

    CompositionDeviceResource(
        std::shared_ptr<OutputResourceList<T>> const& resourceList,
        ID2D1Device6* d2dDevice,
        T const& compositor);

    // OutputResource methods.
    void ReleaseDeviceDependentResources(Output<T> const& output) override;
    void EnsureInitialized(Output<T> const& output) override;

    auto& GetCompositionGraphicsDevice() const noexcept
    {
        return m_graphicsDevice;
    }

private:
    CompositionGraphicsDevice m_graphicsDevice;
    bool m_isInitialized = false;
};
