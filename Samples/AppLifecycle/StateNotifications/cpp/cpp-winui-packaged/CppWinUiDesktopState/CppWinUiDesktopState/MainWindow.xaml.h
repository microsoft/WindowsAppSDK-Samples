// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "MainWindow.g.h"

namespace winrt::CppWinUiDesktopState::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

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
        Windows::Foundation::Collections::IVector<IInspectable> messages;
        Windows::System::DispatcherQueue dispatcherQueue { nullptr };

        void RegisterPowerManagerCallbacks();
        void UnregisterPowerManagerCallbacks();
        void OutputMessage(const WCHAR* message);
        void OutputFormattedMessage(const WCHAR* fmt, ...);

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

        void RegisterButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender, 
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnregisterButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender, 
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::CppWinUiDesktopState::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
