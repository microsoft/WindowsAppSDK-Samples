// Copyright(c) Microsoft Corporation.
// Licensed under the MIT License.

// DynamicDependenciesBasic.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <MddBootstrap.h>

// Used to demo usage of a Windows App SDK api
#include "winrt\Microsoft.Windows.System.Power.h"

// Reference cppwinrt nuget in order to use WinRT apis:
using namespace winrt;
using namespace winrt::Microsoft::Windows::System::Power;

// Constants for dyanmic dependencies lookup
// The following code is for 1.0 Preview 2. 
// Preview tag not needed for GA release
// PCWSTR versionTag{ L"" };
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"preview2" };

int main()
{
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

    // Call a simple Windows App SDK api, and output the result
    // Off = 0, On = 1, Dimmed = 2,
    DisplayStatus dispStatus = PowerManager::DisplayStatus();

    std::cout << "Hello World!\n";
    std::cout << "Power.DisplayStatus is " << (int)dispStatus << "\n";
    
    // Release the DDLM and clean up.
    MddBootstrapShutdown();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
