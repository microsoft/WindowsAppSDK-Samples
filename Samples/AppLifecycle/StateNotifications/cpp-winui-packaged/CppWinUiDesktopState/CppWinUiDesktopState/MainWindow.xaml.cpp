// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Diagnostics;
using namespace winrt::Windows::System;
using namespace winrt::Microsoft::Windows::System::Power;

namespace winrt::CppWinUiDesktopState::implementation
{
    MainWindow::MainWindow()
    {
        dispatcherQueue = &(DispatcherQueue::GetForCurrentThread());
        messages = winrt::single_threaded_observable_vector<IInspectable>();
        InitializeComponent();
        this->StatusListView().ItemsSource(messages);
        this->Title(winrt::hstring(L"CppWinUiDesktopState"));
    }
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::RegisterButton_Click(
    winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    RegisterPowerManagerCallbacks();
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::UnregisterButton_Click(
    winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    UnregisterPowerManagerCallbacks();
}

// Helpers ////////////////////////////////////////////////////////////////////

void winrt::CppWinUiDesktopState::implementation::MainWindow::OutputMessage(const WCHAR* message)
{
    messages.Append(PropertyValue::CreateString(message));
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

// Register/Unregister state/power notification callbacks /////////////////////

void winrt::CppWinUiDesktopState::implementation::MainWindow::RegisterPowerManagerCallbacks()
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

    if (batteryToken && powerToken && powerSourceToken && chargeToken && dischargeToken)
    {
        OutputMessage(L"Successfully registered for state notifications");
    }
    else
    {
        OutputMessage(L"Failed to register for state notifications");
    }
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::UnregisterPowerManagerCallbacks()
{
    OutputMessage(L"Unregistering state notifications");
    PowerManager::BatteryStatusChanged(batteryToken);
    PowerManager::PowerSupplyStatusChanged(powerToken);
    PowerManager::PowerSourceKindChanged(powerSourceToken);
    PowerManager::RemainingChargePercentChanged(chargeToken);
    PowerManager::RemainingDischargeTimeChanged(dischargeToken);
}

///////////////////////////////////////////////////////////////////////////////


// State/power notification callbacks /////////////////////////////////////////

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnBatteryStatusChanged()
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

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnPowerSupplyStatusChanged()
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

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnPowerSourceKindChanged()
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

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnRemainingChargePercentChanged()
{
    OutputMessage(L"Remaining charge percent changed");
    DetermineWorkloads();
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnRemainingDischargeTimeChanged()
{
    OutputMessage(L"Remaining discharge time changed");
    DetermineWorkloads();
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::OnDisplayStatusChanged()
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

void winrt::CppWinUiDesktopState::implementation::MainWindow::DetermineWorkloads()
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

void winrt::CppWinUiDesktopState::implementation::MainWindow::PauseNonCriticalWork()
{
    bWorkInProgress = false;
    OutputMessage(L"paused non-critical work");
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::StartPowerIntensiveWork()
{
    if (!bWorkInProgress)
    {
        bWorkInProgress = true;
        OutputMessage(L"starting power-intensive work");
    }
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::StopUpdatingGraphics()
{
    OutputMessage(L"stopped updating graphics");
}

void winrt::CppWinUiDesktopState::implementation::MainWindow::StartDoingBackgroundWork()
{
    OutputMessage(L"starting background work");
}

///////////////////////////////////////////////////////////////////////////////
