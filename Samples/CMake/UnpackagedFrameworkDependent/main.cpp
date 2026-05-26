// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <windows.h>
#include <sstream>
#include <string>
#include <fstream>

// WinRT base
#include <winrt/Windows.Foundation.h>

// Foundation - EnvironmentManager, PowerManager
#include <winrt/Microsoft.Windows.System.h>
#include <winrt/Microsoft.Windows.System.Power.h>

// DWrite - DWriteCore text rendering engine
#include <dwrite_core.h>

// IXP (InteractiveExperiences) - AppWindowTitleBar customization support
#include <winrt/Microsoft.UI.Windowing.h>

// Widgets - WidgetManager provider API
#include <winrt/Microsoft.Windows.Widgets.Providers.h>

// AI - LanguageModel readiness check
#include <winrt/Microsoft.Windows.AI.h>
#include <winrt/Microsoft.Windows.AI.Text.h>

// ML - Windows ML ExecutionProvider catalog
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

// WinUI - XAML CornerRadiusHelper
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Xaml.h>
#pragma pop_macro("GetCurrentTime")

int WINAPI wWinMain(
    _In_ HINSTANCE,
    _In_opt_ HINSTANCE,
    _In_ PWSTR,
    _In_ int)
{
    wchar_t tempPath[MAX_PATH]{};
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring logPath = std::wstring(tempPath) + L"CMake_UnpackagedFW.log";
    std::wofstream log(logPath, std::ios::trunc);

    try
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);

        std::wostringstream message;
        message << L"Unpackaged Framework-Dependent App (CMake)\n\n";

        //--------------------------------------------------------------------------------------------------------------
        // Foundation
        //--------------------------------------------------------------------------------------------------------------
        message << L"[Foundation]\n";

        if (winrt::Microsoft::Windows::System::EnvironmentManager::IsSupported())
        {
            auto envManager = winrt::Microsoft::Windows::System::EnvironmentManager::GetForProcess();
            auto value = envManager.GetEnvironmentVariable(L"PROCESSOR_ARCHITECTURE");
            message << L"  EnvironmentManager: Supported\n"
                    << L"  PROCESSOR_ARCHITECTURE: " << std::wstring(value) << L"\n";
            log << L"Foundation=Supported" << std::endl;
        }
        else
        {
            message << L"  EnvironmentManager: Not supported\n";
            log << L"Foundation=NotSupported" << std::endl;
        }

        namespace Power = winrt::Microsoft::Windows::System::Power;

        auto batteryStatus = Power::PowerManager::BatteryStatus();
        message << L"  BatteryStatus: ";
        switch (batteryStatus)
        {
        case Power::BatteryStatus::NotPresent:
            message << L"NotPresent";
            break;
        case Power::BatteryStatus::Discharging:
            message << L"Discharging";
            break;
        case Power::BatteryStatus::Idle:
            message << L"Idle";
            break;
        case Power::BatteryStatus::Charging:
            message << L"Charging";
            break;
        default:
            message << L"Unknown";
            break;
        }
        message << L"\n";

        auto powerSupply = Power::PowerManager::PowerSupplyStatus();
        message << L"  PowerSupplyStatus: ";
        switch (powerSupply)
        {
        case Power::PowerSupplyStatus::NotPresent:
            message << L"NotPresent";
            break;
        case Power::PowerSupplyStatus::Inadequate:
            message << L"Inadequate";
            break;
        case Power::PowerSupplyStatus::Adequate:
            message << L"Adequate";
            break;
        default:
            message << L"Unknown";
            break;
        }
        message << L"\n";

        message << L"  RemainingChargePercent: " << Power::PowerManager::RemainingChargePercent() << L"%\n";

        //--------------------------------------------------------------------------------------------------------------
        // DWrite
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[DWrite]\n";

        auto dwriteModule = LoadLibraryW(L"DWriteCore.dll");
        if (dwriteModule)
        {
            using DWriteCoreCreateFactoryFn = HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
            auto createFactory = reinterpret_cast<DWriteCoreCreateFactoryFn>(
                GetProcAddress(dwriteModule, "DWriteCoreCreateFactory")
            );
            if (createFactory)
            {
                winrt::com_ptr<IDWriteFactory> dwriteFactory;
                HRESULT hr = createFactory(
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(IDWriteFactory),
                    reinterpret_cast<IUnknown**>(dwriteFactory.put())
                );
                message << L"  DWriteCoreCreateFactory: " << (SUCCEEDED(hr) ? L"Loaded" : L"Failed") << L"\n";
                log << (SUCCEEDED(hr) ? L"DWrite=Loaded" : L"DWrite=Failed") << std::endl;
            }
            else
            {
                message << L"  DWriteCore: DLL loaded but export not found\n";
                log << L"DWrite=Failed" << std::endl;
            }
        }
        else
        {
            message << L"  DWriteCore: Not available (install WinAppSDK runtime)\n";
            log << L"DWrite=Failed" << std::endl;
        }

        //--------------------------------------------------------------------------------------------------------------
        // IXP (InteractiveExperiences)
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[InteractiveExperiences]\n";

        try
        {
            bool isSupported = winrt::Microsoft::UI::Windowing::AppWindowTitleBar::IsCustomizationSupported();
            message << L"  AppWindowTitleBar.IsCustomizationSupported: " << (isSupported ? L"true" : L"false") << L"\n";
            log << (isSupported ? L"IXP=true" : L"IXP=false") << std::endl;
        }
        catch (winrt::hresult_error const& ex)
        {
            message << L"  AppWindowTitleBar.IsCustomizationSupported: 0x" << std::hex << ex.code() << std::dec << L"\n";
            log << L"IXP=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::dec << std::endl;
        }

        //--------------------------------------------------------------------------------------------------------------
        // Widgets
        // NOTE: WidgetManager requires MSIX package identity AND the WinAppSDK Framework package
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[Widgets]\n";

        try
        {
            auto widgetManager = winrt::Microsoft::Windows::Widgets::Providers::WidgetManager::GetDefault();
            auto widgets = widgetManager.GetWidgetInfos();
            message << L"  WidgetManager.GetWidgetInfos: count = " << widgets.size() << L"\n";
            log << L"Widgets=count_" << widgets.size() << std::endl;
        }
        catch (winrt::hresult_error const& ex)
        {
            message << L"  WidgetManager.GetDefault: 0x" << std::hex << ex.code() << std::dec
                    << L" (requires package identity and framework-dependent deployment)\n";
            log << L"Widgets=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::dec << std::endl;
        }

        //--------------------------------------------------------------------------------------------------------------
        // AI
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[AI]\n";

        try
        {
            auto readyState = winrt::Microsoft::Windows::AI::Text::LanguageModel::GetReadyState();
            std::wstring stateStr;
            switch (readyState)
            {
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::Ready:
                stateStr = L"Ready";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::NotReady:
                stateStr = L"NotReady";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::NotSupportedOnCurrentSystem:
                stateStr = L"NotSupportedOnCurrentSystem";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::DisabledByUser:
                stateStr = L"DisabledByUser";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::CapabilityMissing:
                stateStr = L"CapabilityMissing";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::NotCompatibleWithSystemHardware:
                stateStr = L"NotCompatibleWithSystemHardware";
                break;
            case winrt::Microsoft::Windows::AI::AIFeatureReadyState::OSUpdateNeeded:
                stateStr = L"OSUpdateNeeded";
                break;
            default:
                stateStr = std::to_wstring(static_cast<int32_t>(readyState));
                break;
            }
            message << L"  LanguageModel.GetReadyState: " << stateStr << L"\n";
            log << L"AI=" << stateStr << std::endl;
        }
        catch (winrt::hresult_error const& ex)
        {
            message << L"  LanguageModel.GetReadyState: 0x" << std::hex << ex.code() << std::dec
                    << L" (expected on non-AI hardware)\n";
            log << L"AI=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::dec << std::endl;
        }

        //--------------------------------------------------------------------------------------------------------------
        // ML
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[ML]\n";

        try
        {
            auto catalog = winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
            auto providers = catalog.FindAllProviders();
            message << L"  ExecutionProviderCatalog.FindAllProviders: count = " << providers.size() << L"\n";
            log << L"ML=providers_" << providers.size() << std::endl;
        }
        catch (winrt::hresult_error const& ex)
        {
            message << L"  ExecutionProviderCatalog: 0x" << std::hex << ex.code() << std::dec << L"\n";
            log << L"ML=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::dec << std::endl;
        }

        //--------------------------------------------------------------------------------------------------------------
        // WinUI
        //--------------------------------------------------------------------------------------------------------------
        message << L"\n[WinUI]\n";

        try
        {
            auto cornerRadius = winrt::Microsoft::UI::Xaml::CornerRadiusHelper::FromUniformRadius(4.0);
            message << L"  CornerRadiusHelper.FromUniformRadius(4.0): TopLeft="
                    << cornerRadius.TopLeft << L" BottomRight=" << cornerRadius.BottomRight << L"\n";
            log << L"WinUI=TopLeft_" << cornerRadius.TopLeft
                << L"_BottomRight_" << cornerRadius.BottomRight << std::endl;
        }
        catch (winrt::hresult_error const& ex)
        {
            message << L"  CornerRadiusHelper: 0x" << std::hex << ex.code() << std::dec << L"\n";
            log << L"WinUI=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::dec << std::endl;
        }

        log << L"Result=SUCCESS" << std::endl;
        log.close();

        MessageBoxW(
            nullptr,
            message.str().c_str(),
            L"WinAppSDK CMake Smoke Test",
            MB_OK | MB_ICONINFORMATION
        );
    }
    catch (winrt::hresult_error const& ex)
    {
        log << L"Error=0x" << std::hex << static_cast<int32_t>(ex.code()) << std::endl;
        log.close();
        std::wostringstream err;
        err << L"WinRT error 0x" << std::hex << ex.code() << L"\n" << ex.message().c_str();
        MessageBoxW(
            nullptr,
            err.str().c_str(),
            L"WinAppSDK Error",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    return 0;
}
