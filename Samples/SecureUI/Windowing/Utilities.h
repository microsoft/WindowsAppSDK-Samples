// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <MddBootstrap.h>

namespace Utilities
{
    struct WindowsAppSDKBootstrapperContext
    {
        WindowsAppSDKBootstrapperContext()
        {
            // Take a dependency on Windows App SDK 1.1
            // If using version 1.1 Preview, 
            // replace with majorMinorVersion{ 0x00010001 } and  versionTag{ L"preview" }.
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
