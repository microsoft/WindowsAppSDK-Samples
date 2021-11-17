// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

using namespace std;

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Diagnostics;
using namespace winrt::Windows::System;
using namespace winrt::Microsoft::Windows::System::Power;

// Windows App SDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"" };
const PACKAGE_VERSION minVersion{};

BOOL isWorkInProgress;

winrt::event_token batteryToken;
winrt::event_token powerToken;
winrt::event_token powerSourceToken;
winrt::event_token chargeToken;
winrt::event_token dischargeToken;
winrt::event_token displayToken;
winrt::event_token energyToken;
winrt::event_token powerModeToken;
winrt::event_token userPresenceToken;
winrt::event_token systemSuspendToken;

void RegisterForStateNotifications();
void UnregisterForStateNotifications();
void DetermineWorkloads();
void PauseNonCriticalWork();
void StartPowerIntensiveWork();
void StopUpdatingGraphics();
void StartDoingBackgroundWork();

void OnBatteryStatusChanged();
void OnPowerSupplyStatusChanged();
void OnPowerSourceKindChanged();
void OnDisplayStatusChanged();
void OnRemainingChargePercentChanged();
void OnRemainingDischargeTimeChanged();
void OnEnergySaverStatusChanged();
void OnPowerModeChanged();
void OnUserPresenceStatusChanged();
void OnSystemSuspendStatusChanged();


// Helpers ////////////////////////////////////////////////////////////////////

void OutputMessage(const WCHAR* message)
{
    _putws(message);
}

void OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

///////////////////////////////////////////////////////////////////////////////


int main()
{
    init_apartment();

    // Initialize Windows App SDK for unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, 
            minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    char charOption[2] = { 0 };
    int intOption = 0;
    do
    {
        _putws(L"\nMENU");
        _putws(L"1 - Register for state notifications");
        _putws(L"2 - Unregister for state notifications");
        _putws(L"3 - Quit");
        _putws(L"Select an option: ");

        scanf_s("%1s", charOption, (unsigned)_countof(charOption));
        intOption = atoi(charOption);
        switch (intOption)
        {
        case 1:
            RegisterForStateNotifications();
            break;
        case 2:
            UnregisterForStateNotifications();
            break;
        case 3:
            break;
        default:
            printf("*** Error: %s is not a valid choice ***", charOption);
            break;
        }
    } while (intOption != 3);

    // Uninitialize Windows App SDK.
    MddBootstrapShutdown();
    return 0;
}

// Register/Unregister state/power notification callbacks /////////////////////

void RegisterForStateNotifications()
{
    batteryToken = PowerManager::BatteryStatusChanged([](auto&&...) { OnBatteryStatusChanged(); });
    powerToken = PowerManager::PowerSupplyStatusChanged([](auto&&...) { OnPowerSupplyStatusChanged(); });
    powerSourceToken = PowerManager::PowerSourceKindChanged([](auto&&...) { OnPowerSourceKindChanged(); });
    chargeToken = PowerManager::RemainingChargePercentChanged([](auto&&...) { OnRemainingChargePercentChanged(); });
    dischargeToken = PowerManager::RemainingDischargeTimeChanged([](auto&&...) { OnRemainingDischargeTimeChanged(); });
    displayToken = PowerManager::DisplayStatusChanged([](auto&&...) { OnDisplayStatusChanged(); });
    energyToken = PowerManager::EnergySaverStatusChanged([](auto&&...) { OnEnergySaverStatusChanged(); });
    powerModeToken = PowerManager::EffectivePowerModeChanged([](auto&&...) { OnPowerModeChanged(); });
    userPresenceToken = PowerManager::UserPresenceStatusChanged([](auto&&...) { OnUserPresenceStatusChanged(); });
    systemSuspendToken = PowerManager::SystemSuspendStatusChanged([](auto&&...) { OnSystemSuspendStatusChanged(); });

    if (batteryToken && powerToken && powerSourceToken && chargeToken && dischargeToken
        && displayToken && energyToken && powerModeToken && userPresenceToken && systemSuspendToken)
    {
        OutputMessage(L"Successfully registered for state notifications");
    }
    else
    {
        OutputMessage(L"Failed to register for state notifications");
    }
}

void UnregisterForStateNotifications()
{
    OutputMessage(L"Unregistering state notifications");
    PowerManager::BatteryStatusChanged(batteryToken);
    PowerManager::PowerSupplyStatusChanged(powerToken);
    PowerManager::PowerSourceKindChanged(powerSourceToken);
    PowerManager::RemainingChargePercentChanged(chargeToken);
    PowerManager::RemainingDischargeTimeChanged(dischargeToken);
    PowerManager::DisplayStatusChanged(displayToken);
    PowerManager::EnergySaverStatusChanged(energyToken);
    PowerManager::EffectivePowerModeChanged(powerModeToken);
    PowerManager::UserPresenceStatusChanged(userPresenceToken);
    PowerManager::SystemSuspendStatusChanged(systemSuspendToken);
}

///////////////////////////////////////////////////////////////////////////////


// State/power notification callbacks /////////////////////////////////////////

void OnBatteryStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR status[statusSize];
    wmemset(&(status[0]), 0, statusSize);

    BatteryStatus batteryStatus = PowerManager::BatteryStatus();
    int remainingCharge = PowerManager::RemainingChargePercent();
    switch (batteryStatus)
    {
    case BatteryStatus::Charging:
        wcscpy_s(status, L"Charging");
        break;
    case BatteryStatus::Discharging:
        wcscpy_s(status, L"Discharging");
        break;
    case BatteryStatus::Idle:
        wcscpy_s(status, L"Idle");
        break;
    case BatteryStatus::NotPresent:
        wcscpy_s(status, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Battery status changed: %s, %d%% remaining",
        status, remainingCharge);
    DetermineWorkloads();
}

void OnPowerSupplyStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR status[statusSize];
    wmemset(&(status[0]), 0, statusSize);

    PowerSupplyStatus powerStatus = PowerManager::PowerSupplyStatus();
    switch (powerStatus)
    {
    case PowerSupplyStatus::Adequate:
        wcscpy_s(status, L"Adequate");
        break;
    case PowerSupplyStatus::Inadequate:
        wcscpy_s(status, L"Inadequate");
        break;
    case PowerSupplyStatus::NotPresent:
        wcscpy_s(status, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Power supply status changed: %s", status);
    DetermineWorkloads();
}

void OnPowerSourceKindChanged()
{
    const size_t statusSize = 16;
    WCHAR status[statusSize];
    wmemset(&(status[0]), 0, statusSize);

    PowerSourceKind powerSource = PowerManager::PowerSourceKind();
    switch (powerSource)
    {
    case PowerSourceKind::AC:
        wcscpy_s(status, L"AC");
        break;
    case PowerSourceKind::DC:
        wcscpy_s(status, L"DC");
        break;
    }

    OutputFormattedMessage(
        L"Power source kind changed: %s", status);
    DetermineWorkloads();
}

void OnRemainingChargePercentChanged()
{
    OutputMessage(L"Remaining charge percent changed");
    DetermineWorkloads();
}

void OnRemainingDischargeTimeChanged()
{
    OutputMessage(L"Remaining discharge time changed");
    DetermineWorkloads();
}

void OnDisplayStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR status[statusSize];
    wmemset(&(status[0]), 0, statusSize);

    DisplayStatus displayStatus = PowerManager::DisplayStatus();
    switch (displayStatus)
    {
    case DisplayStatus::Dimmed:
        wcscpy_s(status, L"Dimmed");
        break;
    case DisplayStatus::Off:
        wcscpy_s(status, L"Off");
        break;
    case DisplayStatus::On:
        wcscpy_s(status, L"On");
        break;
    }

    OutputFormattedMessage(
        L"Display status changed: %s", status);
    if (displayStatus == DisplayStatus::Off)
    {
        // The screen is off, let's stop rendering foreground graphics,
        // and instead kick off some background work now.
        StopUpdatingGraphics();
        StartDoingBackgroundWork();
    }
}

void OnEnergySaverStatusChanged()
{
    OutputMessage(L"Energy saver status changed");
    DetermineWorkloads();
}

void OnPowerModeChanged()
{
    OutputMessage(L"Power mode changed");
    DetermineWorkloads();
}

void OnUserPresenceStatusChanged()
{
    OutputMessage(L"User presence status changed");
    DetermineWorkloads();
}

void OnSystemSuspendStatusChanged()
{
    OutputMessage(L"System suspend status changed");
    DetermineWorkloads();
}

void DetermineWorkloads()
{
    BatteryStatus batteryStatus = PowerManager::BatteryStatus();
    int remainingCharge = PowerManager::RemainingChargePercent();
    PowerSupplyStatus powerStatus = PowerManager::PowerSupplyStatus();
    PowerSourceKind powerSource = PowerManager::PowerSourceKind();

    if ((powerSource == PowerSourceKind::DC
        && batteryStatus == BatteryStatus::Discharging
        && remainingCharge < 25)
        || (powerSource == PowerSourceKind::AC
            && powerStatus == PowerSupplyStatus::Inadequate))
    {
        // The device is not in a good battery/power state, 
        // so we should pause any non-critical work.
        PauseNonCriticalWork();
    }
    else if ((batteryStatus != BatteryStatus::Discharging && remainingCharge > 75)
        && powerStatus != PowerSupplyStatus::Inadequate)
    {
        // The device is in good battery/power state,
        // so let's kick of some high-power work.
        StartPowerIntensiveWork();
    }
}

///////////////////////////////////////////////////////////////////////////////


// Simulate starting/stopping work ////////////////////////////////////////////

void PauseNonCriticalWork()
{
    isWorkInProgress = false;
    OutputMessage(L"paused non-critical work");
}

void StartPowerIntensiveWork()
{
    if (!isWorkInProgress)
    {
        isWorkInProgress = true;
        OutputMessage(L"starting power-intensive work");
    }
}

void StopUpdatingGraphics()
{
    OutputMessage(L"stopped updating graphics");
}

void StartDoingBackgroundWork()
{
    OutputMessage(L"starting background work");
}

///////////////////////////////////////////////////////////////////////////////
