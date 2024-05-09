// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace winrt::DrawingIslandComponents::implementation
{
    struct DeviceLostEventArgs
    {
        DeviceLostEventArgs(IDirect3DDevice const& device) : m_device(device) {}
        IDirect3DDevice Device() { return m_device; }
        static DeviceLostEventArgs Create(IDirect3DDevice const& device) { return DeviceLostEventArgs{ device }; }

    private:
        IDirect3DDevice m_device;
    };

    struct DeviceLostHelper
    {
        DeviceLostHelper() = default;

        ~DeviceLostHelper()
        {
            StopWatchingCurrentDevice();
            m_onDeviceLostHandler = nullptr;
        }

        IDirect3DDevice CurrentlyWatchedDevice()
        {
            return m_device;
        }

        void WatchDevice(winrt::com_ptr<::IDXGIDevice> const& dxgiDevice);

        void StopWatchingCurrentDevice();

        void DeviceLost(winrt::delegate<DeviceLostHelper const*, DeviceLostEventArgs const&> const& handler)
        {
            m_deviceLost = handler;
        }

        winrt::delegate<DeviceLostHelper const*, DeviceLostEventArgs const&> m_deviceLost;

    private:
        void RaiseDeviceLostEvent(IDirect3DDevice const& oldDevice)
        {
            m_deviceLost(this, DeviceLostEventArgs::Create(oldDevice));
        }

        static void CALLBACK OnDeviceLost(PTP_CALLBACK_INSTANCE /* instance */, PVOID context, PTP_WAIT /* wait */, TP_WAIT_RESULT /* waitResult */);

    private:
        IDirect3DDevice m_device;
        winrt::com_ptr<::IDXGIDevice> m_dxgiDevice;
        PTP_WAIT m_onDeviceLostHandler{ nullptr };
        winrt::handle m_eventHandle;
        DWORD m_cookie{ 0 };
    };
}
