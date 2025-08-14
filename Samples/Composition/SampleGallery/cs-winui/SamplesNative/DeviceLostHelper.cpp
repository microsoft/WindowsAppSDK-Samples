﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
// THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*********************************************************

#include "pch.h"
#include "DeviceLostHelper.h"

using namespace SamplesNative;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Graphics::Display;


DeviceLostHelper::DeviceLostHelper()
{
    OnDeviceLostHandler = NULL;
    m_eventHandle = NULL;
    m_cookie = NULL;
}

DeviceLostHelper::~DeviceLostHelper()
{
    StopWatchingCurrentDevice();
    OnDeviceLostHandler = NULL;
}

void
DeviceLostHelper::WatchDevice(IDirect3DDevice^ device)
{
    // Stop listening for device lost if we currently are
    StopWatchingCurrentDevice();

    // Set the current device to the new device
    m_device = device;

    // Get the DXGI Device
    ComPtr<IDXGIDevice> dxgiDevice;
    __abi_ThrowIfFailed(GetDXGIInterface(device, dxgiDevice.GetAddressOf()));

    // QI For the ID3D11Device4 interface
    ComPtr<ID3D11Device4> d3dDevice;
    __abi_ThrowIfFailed(dxgiDevice.As(&d3dDevice));

    // Create a wait struct
    OnDeviceLostHandler = {};
    OnDeviceLostHandler = CreateThreadpoolWait(DeviceLostHelper::OnDeviceLost, (PVOID)this, NULL);

    // Create a handle and a cookie
    m_eventHandle = CreateEvent(NULL, FALSE, FALSE, nullptr);
    m_cookie = NULL;

    // Register for device lost
    SetThreadpoolWait(OnDeviceLostHandler, m_eventHandle, NULL);
    __abi_ThrowIfFailed(d3dDevice->RegisterDeviceRemovedEvent(m_eventHandle, &m_cookie));
}

void
DeviceLostHelper::StopWatchingCurrentDevice()
{
    if (m_device)
    {
        // Get the DXGI Device
        ComPtr<IDXGIDevice> dxgiDevice;
        __abi_ThrowIfFailed(GetDXGIInterface(m_device, dxgiDevice.GetAddressOf()));

        // QI For the ID3D11Device4 interface
        ComPtr<ID3D11Device4> d3dDevice;
        __abi_ThrowIfFailed(dxgiDevice.As(&d3dDevice));

        // Unregister from device lost
        CloseThreadpoolWait(OnDeviceLostHandler);
        d3dDevice->UnregisterDeviceRemoved(m_cookie);

        // Clear member variables
        OnDeviceLostHandler = NULL;
        CloseHandle(m_eventHandle);
        m_eventHandle = NULL;
        m_cookie = NULL;
        m_device = nullptr;
    }
}

void
DeviceLostHelper::RaiseDeviceLostEvent(IDirect3DDevice^ oldDevice)
{
    DeviceLost(this, DeviceLostEventArgs::Create(oldDevice));
}

void CALLBACK
DeviceLostHelper::OnDeviceLost(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WAIT wait, TP_WAIT_RESULT waitResult)
{
    auto deviceLostHelper = reinterpret_cast<DeviceLostHelper^>(context);
    auto oldDevice = deviceLostHelper->m_device;
    deviceLostHelper->StopWatchingCurrentDevice();

    deviceLostHelper->RaiseDeviceLostEvent(oldDevice);
}
