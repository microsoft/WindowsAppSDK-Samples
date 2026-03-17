// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <windows.h>
#include <sstream>
#include <string>

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.System.h>
#include <winrt/Microsoft.Windows.System.Power.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    try
    {
        winrt::init_apartment();

        std::wostringstream message;
        message << L"Unpackaged Self-Contained App (CMake)\n\n";

        if (winrt::Microsoft::Windows::System::EnvironmentManager::IsSupported())
        {
            auto envManager = winrt::Microsoft::Windows::System::EnvironmentManager::GetForProcess();
            auto value = envManager.GetEnvironmentVariable(L"PROCESSOR_ARCHITECTURE");
            message << L"EnvironmentManager: Supported\n";
            message << L"PROCESSOR_ARCHITECTURE: " << std::wstring(value) << L"\n";
        }
        else
        {
            message << L"EnvironmentManager: Not supported on this OS version\n";
        }

        // Use WinAppSDK PowerManager to query battery and power state
        namespace Power = winrt::Microsoft::Windows::System::Power;

        message << L"\n";
        auto batteryStatus = Power::PowerManager::BatteryStatus();
        message << L"PowerManager.BatteryStatus: ";
        switch (batteryStatus)
        {
        case Power::BatteryStatus::NotPresent:   message << L"NotPresent";   break;
        case Power::BatteryStatus::Discharging:  message << L"Discharging";  break;
        case Power::BatteryStatus::Idle:         message << L"Idle";         break;
        case Power::BatteryStatus::Charging:     message << L"Charging";     break;
        default:                                 message << L"Unknown";      break;
        }
        message << L"\n";

        auto powerSupply = Power::PowerManager::PowerSupplyStatus();
        message << L"PowerManager.PowerSupplyStatus: ";
        switch (powerSupply)
        {
        case Power::PowerSupplyStatus::NotPresent:  message << L"NotPresent";  break;
        case Power::PowerSupplyStatus::Inadequate:  message << L"Inadequate";  break;
        case Power::PowerSupplyStatus::Adequate:    message << L"Adequate";    break;
        default:                                    message << L"Unknown";     break;
        }
        message << L"\n";

        message << L"PowerManager.RemainingChargePercent: "
                << Power::PowerManager::RemainingChargePercent() << L"%\n";

        MessageBoxW(nullptr, message.str().c_str(), L"WinAppSDK CMake Test", MB_OK | MB_ICONINFORMATION);
    }
    catch (winrt::hresult_error const& ex)
    {
        std::wostringstream err;
        err << L"WinRT error 0x" << std::hex << ex.code() << L"\n" << ex.message().c_str();
        MessageBoxW(nullptr, err.str().c_str(), L"WinAppSDK Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
