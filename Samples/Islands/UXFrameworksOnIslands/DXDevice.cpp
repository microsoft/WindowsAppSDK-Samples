// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "Output.h"

DXDevice::DXDevice()
{
    // Create the D2D factory.
    winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_d2dFactory.put()));

    InitializeDevice();
}

void DXDevice::RecreateDevice()
{
    // Release resources used by the old device.
    m_d3dDevice = nullptr;
    m_d2dDevice = nullptr;
    m_deviceContext = nullptr;

    // Create the new device.
    InitializeDevice();
}

void DXDevice::InitializeDevice()
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
    winrt::com_ptr<ID3D11Device> d3dDevice;
    winrt::com_ptr<ID3D11DeviceContext> d3dImmediateContext;
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

    // Obtain the underlying DXGI device of the Direct3D device.
    auto dxgiDevice = d3dDevice.as<IDXGIDevice>();

    // Create the Direct2D device object.
    D2D1_FACTORY_OPTIONS d2d1FactoryOptions{ D2D1_DEBUG_LEVEL_NONE };
    winrt::com_ptr<ID2D1Device6> d2dDevice;
    winrt::check_hresult(m_d2dFactory->CreateDevice(
        dxgiDevice.get(), 
        d2dDevice.put()));

    // Create the Direct2D device context.
    winrt::com_ptr<ID2D1DeviceContext5> deviceContext;
    winrt::check_hresult(d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, 
        deviceContext.put()));

    // Commit changes.
    m_d3dDevice = d3dDevice.as<ID3D11Device4>();
    m_d2dDevice = std::move(d2dDevice);
    m_deviceContext = std::move(deviceContext);
}

