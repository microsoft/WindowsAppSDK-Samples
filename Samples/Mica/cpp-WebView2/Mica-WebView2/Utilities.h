// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <DispatcherQueue.h>
#include <MddBootstrap.h>

#include <winrt/Windows.System.h>

namespace Utilities
{
    inline auto CreateDispatcherQueueControllerForCurrentThread()
    {
        namespace abi = ABI::Windows::System;

        DispatcherQueueOptions options
        {
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_NONE
        };

        winrt::Windows::System::DispatcherQueueController controller{ nullptr };
        winrt::check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(controller))));
        return controller;
    }

    struct WindowsAppSDKBootstrapperContext
    {
        WindowsAppSDKBootstrapperContext()
        {
            // Take a dependency on Windows App SDK 1.0, if using 1.0 Experimental,
            // replace with versionTag{ L"experimental1" }. If using version 0.8 Preview, 
            // replace with majorMinorVersion{ 0x00000008 } and  versionTag{ L"preview" }.
            const UINT32 majorMinorVersion{ 0x00010000 };
            PCWSTR versionTag{ L"" };
            const PACKAGE_VERSION minVersion{};

            winrt::check_hresult(MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion));
        }

        ~WindowsAppSDKBootstrapperContext()
        {
            // Release the DDLM and clean up.
            MddBootstrapShutdown();
        }
    };
}
