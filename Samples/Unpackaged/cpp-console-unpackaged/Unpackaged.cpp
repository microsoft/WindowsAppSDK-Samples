// Copyright(c) Microsoft Corporation.
// Licensed under the MIT License.
//
// DynamicDependenciesBasic.cpp :
// This file shows the headers and usings required to call apis in the Windows App SDK.
// To run with auto-initialization of the Windows App Runtime, run as-is
// To explicitly control initialization:
//  - edit project file and remove WindowsPackageType=None
//  - uncomment the code below with WINAPPSDK-INITIALIZE: tags
//

#include <iostream>
#include <windows.h>
#include <MddBootstrap.h>
#include <wil/result.h>

// Include the header to demo usage of a Windows App SDK API
#include "winrt\Microsoft.Windows.System.Power.h"
#include "Unpackaged.h"

namespace winrt
{
    using namespace winrt::Microsoft::Windows::System::Power;
}

int main()
{
    // WINAPPSDK-INITIALIZE: uncomment code below to have explicit control of initialization
    //THROW_IF_FAILED(ExplicitlyInitializeWinAppRuntime());

    // Call a simple Windows App SDK API, and output the result
    auto dispStatus = winrt::PowerManager::DisplayStatus();

    std::cout << "Hello World!\n";
    std::cout << "Power.DisplayStatus is " << (int)dispStatus << "\n";

    // WINAPPSDK-INITIALIZE: uncomment code below to control initialization
    //ExplicitlyCleanUpWinAppRuntime();

}

// Method to handle any explicit initialization of Windows App Runtime
// Not needed if using WindowsPackageType=None in project properties
HRESULT ExplicitlyInitializeWinAppRuntime()
{
    // Constants for dynamic dependencies lookup.
    // Preview tag not needed for GA release, eg: versionTag{ L"" };
    const UINT32 majorMinorVersion{ 0x00010000 };
    PCWSTR versionTag{ L"" };

    // Initialize values for the call to the Dynamic Dependencies bootstrapper
    const PACKAGE_VERSION minVersion{};
    const HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };

    // Check the return code for errors. If there is an error, display the result.
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    return S_OK;
}

// Method to handle any clean up required for explicity initialization of Windows App Runtime
void ExplicitlyCleanUpWinAppRuntime()
{
    // Release the DDLM and clean up.
    MddBootstrapShutdown();
}
