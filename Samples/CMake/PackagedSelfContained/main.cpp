// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <windows.h>
#include <appmodel.h>
#include <sstream>
#include <string>
#include <fstream>

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.System.h>
#include <winrt/Microsoft.Windows.System.Power.h>
#include <dwrite_core.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    // Write results to a log file in the temp directory
    wchar_t tempPath[MAX_PATH]{};
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring logPath = std::wstring(tempPath) + L"CMake_PackagedSC.log";
    std::wofstream log(logPath, std::ios::trunc);

    try
    {
        winrt::init_apartment();

        std::wostringstream message;
        message << L"Packaged Self-Contained App (CMake)\n\n";

        // Check package identity
        UINT32 nameLen = 0;
        auto rc = GetCurrentPackageFullName(&nameLen, nullptr);
        if (rc == APPMODEL_ERROR_NO_PACKAGE)
        {
            message << L"Package Identity: NONE\n";
            log << L"PackageIdentity=NONE" << std::endl;
        }
        else
        {
            std::wstring pkgName(nameLen, L'\0');
            GetCurrentPackageFullName(&nameLen, pkgName.data());
            // Remove trailing null that GetCurrentPackageFullName includes in the length
            if (!pkgName.empty() && pkgName.back() == L'\0')
            {
                pkgName.pop_back();
            }
            message << L"Package Identity: " << pkgName << L"\n";
            log << L"PackageIdentity=" << pkgName << std::endl;
        }

        // Use WinAppSDK API - try EnvironmentManager, fall back to AppLifecycle
        bool apiWorked = false;

        // Try EnvironmentManager
        if (winrt::Microsoft::Windows::System::EnvironmentManager::IsSupported())
        {
            auto envManager = winrt::Microsoft::Windows::System::EnvironmentManager::GetForProcess();
            auto value = envManager.GetEnvironmentVariable(L"PROCESSOR_ARCHITECTURE");
            message << L"EnvironmentManager: Supported\n";
            message << L"PROCESSOR_ARCHITECTURE: " << std::wstring(value) << L"\n";
            log << L"EnvironmentManager=Supported" << std::endl;
            log << L"PROCESSOR_ARCHITECTURE=" << std::wstring(value) << std::endl;
            apiWorked = true;
        }
        else
        {
            message << L"EnvironmentManager: Not supported\n";
            log << L"EnvironmentManager=NotSupported" << std::endl;
        }

        // Always show something from WinAppSDK to prove it loaded
        wchar_t arch[64]{};
        GetEnvironmentVariableW(L"PROCESSOR_ARCHITECTURE", arch, 64);
        message << L"Win32 PROCESSOR_ARCHITECTURE: " << arch << L"\n";

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

        // DWrite: DWriteCore API (proves DWrite component package works)
        winrt::com_ptr<IDWriteFactory> dwriteFactory;
        HRESULT hr = DWriteCoreCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(dwriteFactory.put()));
        if (SUCCEEDED(hr))
        {
            message << L"DWriteCore: Loaded\n";
        }
        else
        {
            message << L"DWriteCore: Failed (0x" << std::hex << hr << L")\n";
        }

        log << L"Result=SUCCESS" << std::endl;
        log.close();
        MessageBoxW(nullptr, message.str().c_str(), L"WinAppSDK CMake Test", MB_OK | MB_ICONINFORMATION);
    }
    catch (winrt::hresult_error const& ex)
    {
        log << L"Error=0x" << std::hex << ex.code() << std::endl;
        log.close();
        std::wostringstream err;
        err << L"WinRT error 0x" << std::hex << ex.code() << L"\n" << ex.message().c_str();
        MessageBoxW(nullptr, err.str().c_str(), L"WinAppSDK Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
