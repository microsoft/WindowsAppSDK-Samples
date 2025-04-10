// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#include "OutputResource.h"
#include "DXDevice.h"
#include "CompositionDeviceResource.h"
#include "Matrix2x2.h"

// Encapsulates objects used to render output for a particular island.
// Use the LiftedOutputResource typedef for lifted islands.
// Use the SystemOutputResource typedef for system islands.
template<class T>
class Output
{
public:
    Output(T const& compositor, std::shared_ptr<SettingCollection> const& settings);
    ~Output();

    Output(Output<T> const&) = delete;
    void operator=(Output<T> const&) = delete;

    auto& GetSettings() const noexcept { return m_settings; }
    bool GetSetting(SettingId id) const noexcept { return m_settings->GetSetting(id); }

    auto& GetResourceList() const noexcept { return m_resourceList; }
    auto& GetCompositor() const noexcept { return m_compositor; }
    auto& GetDXDevice() const noexcept { return m_dxDevice; }
    auto& GetCompositionGraphicsDevice() const noexcept { return m_compositionDevice.GetCompositionGraphicsDevice(); }

    auto& GetRasterizationTransform() const noexcept { return m_rasterizationTransform; }
    void SetRasterizationTransform(Matrix2x2 const& value);

private:
    winrt::fire_and_forget RegisterForDeviceLost();
    void UnregisterFromDeviceLost();

    std::shared_ptr<OutputResourceList<T>> m_resourceList = std::make_shared<OutputResourceList<T>>();
    T m_compositor;
    std::shared_ptr<SettingCollection> m_settings;
    DXDevice m_dxDevice;
    CompositionDeviceResource<T> m_compositionDevice;

    winrt::com_ptr<ID3D11Device4> m_registeredDevice;
    wil::unique_event m_deviceRemovedEventHandle{ wil::EventOptions::ManualReset };
    DWORD m_deviceRemovedEventRegistrationCookie = 0;

    Matrix2x2 m_rasterizationTransform;
};
