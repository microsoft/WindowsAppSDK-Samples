// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

class DXDevice final
{
public:
    DXDevice();

    void RecreateDevice();

    auto& GetD3DDevice() const noexcept
    {
        return m_d3dDevice;
    }

    auto& GetD2DDevice() const noexcept
    {
        return m_d2dDevice;
    }

    auto& GetDeviceContext() const noexcept
    {
        return m_deviceContext;
    }

private:
    void InitializeDevice();

    winrt::com_ptr<ID2D1Factory7> m_d2dFactory;
    winrt::com_ptr<ID3D11Device4> m_d3dDevice;
    winrt::com_ptr<ID2D1Device6> m_d2dDevice;
    winrt::com_ptr<ID2D1DeviceContext5> m_deviceContext;
};
