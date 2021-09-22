﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

using namespace std;

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Diagnostics;
using namespace winrt::Windows::System;
using namespace winrt::Microsoft::Windows::System::Power;

// WASDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"preview1" };
const PACKAGE_VERSION minVersion{};

BOOL bWorkInProgress;

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

    char char_choice[2];
    int int_choice = 0;
    do
    {
        _putws(L"\nMENU");
        _putws(L"1 - Register for state notifications");
        _putws(L"2 - Unregister for state notifications");
        _putws(L"3 - Quit");
        _putws(L"Select an option: ");

        scanf("%s", char_choice);
        int_choice = atoi(char_choice);
        switch (int_choice)
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
            printf("*** Error: %s is not a valid choice ***", char_choice);
            break;
        }
    } while (int_choice != 3);

    // Uninitialize Windows App SDK.
    MddBootstrapShutdown();
    return 0;
}

// Register/Unregister state/power notification callbacks /////////////////////

void RegisterForStateNotifications()
{
    batteryToken = PowerManager::BatteryStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnBatteryStatusChanged(); });
    powerToken = PowerManager::PowerSupplyStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnPowerSupplyStatusChanged(); });
    powerSourceToken = PowerManager::PowerSourceKindChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnPowerSourceKindChanged(); });
    chargeToken = PowerManager::RemainingChargePercentChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnRemainingChargePercentChanged(); });
    dischargeToken = PowerManager::RemainingDischargeTimeChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnRemainingDischargeTimeChanged(); });
    displayToken = PowerManager::DisplayStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnDisplayStatusChanged(); });
    energyToken = PowerManager::EnergySaverStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnEnergySaverStatusChanged(); });
    powerModeToken = PowerManager::EffectivePowerModeChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnPowerModeChanged(); });
    userPresenceToken = PowerManager::UserPresenceStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnUserPresenceStatusChanged(); });
    systemSuspendToken = PowerManager::SystemSuspendStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnSystemSuspendStatusChanged(); });

    if (batteryToken && powerToken && powerSourceToken && chargeToken && dischargeToken)
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
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    BatteryStatus batteryStatus = PowerManager::BatteryStatus();
    int remainingCharge = PowerManager::RemainingChargePercent();
    switch (batteryStatus)
    {
    case BatteryStatus::Charging:
        wcscpy_s(szStatus, L"Charging");
        break;
    case BatteryStatus::Discharging:
        wcscpy_s(szStatus, L"Discharging");
        break;
    case BatteryStatus::Idle:
        wcscpy_s(szStatus, L"Idle");
        break;
    case BatteryStatus::NotPresent:
        wcscpy_s(szStatus, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Battery status changed: %s, %d%% remaining",
        szStatus, remainingCharge);
    DetermineWorkloads();
}

void OnPowerSupplyStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    PowerSupplyStatus powerStatus = PowerManager::PowerSupplyStatus();
    switch (powerStatus)
    {
    case PowerSupplyStatus::Adequate:
        wcscpy_s(szStatus, L"Adequate");
        break;
    case PowerSupplyStatus::Inadequate:
        wcscpy_s(szStatus, L"Inadequate");
        break;
    case PowerSupplyStatus::NotPresent:
        wcscpy_s(szStatus, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Power supply status changed: %s", szStatus);
    DetermineWorkloads();
}

void OnPowerSourceKindChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    PowerSourceKind powerSource = PowerManager::PowerSourceKind();
    switch (powerSource)
    {
    case PowerSourceKind::AC:
        wcscpy_s(szStatus, L"AC");
        break;
    case PowerSourceKind::DC:
        wcscpy_s(szStatus, L"DC");
        break;
    }

    OutputFormattedMessage(
        L"Power source kind changed: %s", szStatus);
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
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    DisplayStatus displayStatus = PowerManager::DisplayStatus();
    switch (displayStatus)
    {
    case DisplayStatus::Dimmed:
        wcscpy_s(szStatus, L"Dimmed");
        break;
    case DisplayStatus::Off:
        wcscpy_s(szStatus, L"Off");
        break;
    case DisplayStatus::On:
        wcscpy_s(szStatus, L"On");
        break;
    }

    OutputFormattedMessage(
        L"Display status changed: %s", szStatus);
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
    bWorkInProgress = false;
    OutputMessage(L"paused non-critical work");
}

void StartPowerIntensiveWork()
{
    if (!bWorkInProgress)
    {
        bWorkInProgress = true;
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
