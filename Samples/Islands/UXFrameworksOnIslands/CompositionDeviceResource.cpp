// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "Output.h"

inline winrt::CompositionGraphicsDevice CreateCompositionGraphicsDevice(ID2D1Device6* d2dDevice, winrt::Compositor const& compositor)
{
    auto compositorInterop = compositor.as<winrt::ICompositorInterop>();
    winrt::ICompositionGraphicsDevice graphicsDevice;
    winrt::check_hresult(compositorInterop->CreateGraphicsDevice(d2dDevice, &graphicsDevice));
    return graphicsDevice.as<winrt::CompositionGraphicsDevice>();
}

inline winrt::WUC::CompositionGraphicsDevice CreateCompositionGraphicsDevice(ID2D1Device6* d2dDevice, winrt::WUC::Compositor const& compositor)
{
    auto compositorInterop = compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionGraphicsDevice> graphicsDevice;
    winrt::check_hresult(compositorInterop->CreateGraphicsDevice(d2dDevice, graphicsDevice.put()));
    return graphicsDevice.as<winrt::WUC::CompositionGraphicsDevice>();
}

template <class T>
CompositionDeviceResource<T>::CompositionDeviceResource(
        std::shared_ptr<OutputResourceList<T>> const& resourceList, 
        ID2D1Device6* d2dDevice, 
        T const& compositor) :
    OutputResource<T>(resourceList),
    m_graphicsDevice(CreateCompositionGraphicsDevice(d2dDevice, compositor))
{
}

template <class T>
void CompositionDeviceResource<T>::CreateDeviceDependentResources(Output<T> const& output)
{
    // Restore the composition graphics device to health by pointing to the new Direct2D device.
    auto deviceInterop = m_graphicsDevice.as<ICompositionGraphicsDeviceInterop>();
    winrt::check_hresult(deviceInterop->SetRenderingDevice(output.GetDXDevice().GetD2DDevice().get()));
}

// Explicit template instantiation.
template class CompositionDeviceResource<winrt::Compositor>;
template class CompositionDeviceResource<winrt::WUC::Compositor>;
