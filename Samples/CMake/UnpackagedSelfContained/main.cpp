// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <windows.h>
#include <sstream>
#include <string>

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.System.h>
#include <winrt/Microsoft.Windows.System.Power.h>
#include <dwrite_core.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    try
    {
        winrt::init_apartment();

        std::wostringstream message;
        message << L"Unpackaged Self-Contained App (CMake)\n\n";

        // Foundation: EnvironmentManager API
        if (winrt::Microsoft::Windows::System::EnvironmentManager::IsSupported())
        {
            auto envManager = winrt::Microsoft::Windows::System::EnvironmentManager::GetForProcess();
            auto value = envManager.GetEnvironmentVariable(L"PROCESSOR_ARCHITECTURE");
            message << L"EnvironmentManager: Supported\n";
            message << L"PROCESSOR_ARCHITECTURE: " << std::wstring(value) << L"\n";
        }
        else
        {
            message << L"EnvironmentManager: Not supported\n";
        }

        // Foundation: PowerManager API
        namespace Power = winrt::Microsoft::Windows::System::Power;
        message << L"BatteryStatus: ";
        switch (Power::PowerManager::BatteryStatus())
        {
        case Power::BatteryStatus::NotPresent:  message << L"NotPresent";  break;
        case Power::BatteryStatus::Discharging: message << L"Discharging"; break;
        case Power::BatteryStatus::Idle:        message << L"Idle";        break;
        case Power::BatteryStatus::Charging:    message << L"Charging";    break;
        default:                                message << L"Unknown";     break;
        }
        message << L"\n";

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
