// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "Output.h"

template<class T>
Output<T>::Output(T const& compositor, std::shared_ptr<SettingCollection> const& settings) :
    m_compositor(compositor),
    m_settings(settings),
    m_compositionDevice(m_resourceList, m_dxDevice.GetD2DDevice().get(), compositor)
{
    RegisterForDeviceLost();
}

template<class T>
Output<T>::~Output()
{
    UnregisterFromDeviceLost();
}

template<class T>
void Output<T>::SetRasterizationTransform(Matrix2x2 const& value)
{
    m_rasterizationTransform = value;
}

template<class T>
winrt::fire_and_forget Output<T>::RegisterForDeviceLost()
{
    auto d3dDevice = m_dxDevice.GetD3DDevice();

    WINRT_ASSERT(m_registeredDevice == nullptr);
    WINRT_ASSERT(d3dDevice != nullptr);

    winrt::check_hresult(d3dDevice->RegisterDeviceRemovedEvent(
        m_deviceRemovedEventHandle.get(),
        &m_deviceRemovedEventRegistrationCookie));

    m_registeredDevice = std::move(d3dDevice);

    // Acquire a local reference to the DispatcherQueue.
    auto localDispatcherQueue = m_compositor.DispatcherQueue();

    co_await winrt::resume_on_signal(m_deviceRemovedEventHandle.get());

    // The await resume comes on a background thread, so use the DispatcherQueue to
    // marshal the call to the UI thread.
    localDispatcherQueue.TryEnqueue([this] {

        UnregisterFromDeviceLost();

        // Tell each resource object to release resources.
        m_resourceList->ReleaseDeviceDependentResources(*this);

        // Recreate the Direct3D and Direct2D devices.
        m_dxDevice.RecreateDevice();

        // Tell each resource object to recreate resources.
        m_resourceList->EnsureInitialized(*this);

        // Listen for device lost on the new device.
        RegisterForDeviceLost();
    });
}

template<class T>
void Output<T>::UnregisterFromDeviceLost()
{
    if (m_registeredDevice != nullptr)
    {
        m_registeredDevice->UnregisterDeviceRemoved(m_deviceRemovedEventRegistrationCookie);
        m_deviceRemovedEventHandle.ResetEvent();
        m_registeredDevice = nullptr;
    }
}

// Explicit template instantiation.
template class Output<winrt::Compositor>;
template class Output<winrt::WUC::Compositor>;
