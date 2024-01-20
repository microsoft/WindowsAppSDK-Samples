// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using Microsoft.Windows.System.Power;
using global::Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsConsoleState
{
    class Program
    {

        #region Init

        // Windows App SDK version.
        static uint majorMinorVersion = 0x00010000;
        private static bool isWorkInProgress;

        static void Main(string[] args)
        {
            // Initialize Windows App SDK for unpackaged apps.            
            int result = 0;
            
            if (Bootstrap.TryInitialize(majorMinorVersion, out result))
            {
                int choice = 0;
                while (choice != 3)
                {
                    Console.WriteLine("\nMENU");
                    Console.WriteLine("1 - Register for state notifications");
                    Console.WriteLine("2 - Unregister for state notifications");
                    Console.WriteLine("3 - Quit");
                    Console.WriteLine("Select an option: ");
                    if (int.TryParse(Console.ReadLine(), out int tmp))
                    {
                        choice = tmp;
                        switch (choice)
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
                                Console.WriteLine($"*** Error: {choice} is not a valid choice ***");
                                break;
                        }
                    }
                }

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
        }

        private static void OutputMessage(string message)
        {
            Console.WriteLine(message);
        }

        #endregion


        #region Register/Unregister callbacks

        private static void RegisterForStateNotifications()
        {
            OutputMessage("Registering for state notifications");
            PowerManager.BatteryStatusChanged += PowerManager_BatteryStatusChanged;
            PowerManager.PowerSupplyStatusChanged += PowerManager_PowerSupplyStatusChanged;
            PowerManager.PowerSourceKindChanged += PowerManager_PowerSourceKindChanged;
            PowerManager.RemainingChargePercentChanged += PowerManager_RemainingChargePercentChanged;
            PowerManager.RemainingDischargeTimeChanged += PowerManager_RemainingDischargeTimeChanged;
            PowerManager.DisplayStatusChanged += PowerManager_DisplayStatusChanged;
            PowerManager.EnergySaverStatusChanged += PowerManager_EnergySaverStatusChanged;
            PowerManager.EffectivePowerModeChanged += PowerManager_EffectivePowerModeChanged;
            PowerManager.UserPresenceStatusChanged += PowerManager_UserPresenceStatusChanged;
            PowerManager.SystemSuspendStatusChanged += PowerManager_SystemSuspendStatusChanged;
        }

        private static void UnregisterForStateNotifications()
        {
            OutputMessage("Unregistering for state notifications");
            PowerManager.BatteryStatusChanged -= PowerManager_BatteryStatusChanged;
            PowerManager.PowerSupplyStatusChanged -= PowerManager_PowerSupplyStatusChanged;
            PowerManager.PowerSourceKindChanged -= PowerManager_PowerSourceKindChanged;
            PowerManager.RemainingChargePercentChanged -= PowerManager_RemainingChargePercentChanged;
            PowerManager.RemainingDischargeTimeChanged -= PowerManager_RemainingDischargeTimeChanged;
            PowerManager.DisplayStatusChanged -= PowerManager_DisplayStatusChanged;
            PowerManager.EnergySaverStatusChanged -= PowerManager_EnergySaverStatusChanged;
            PowerManager.EffectivePowerModeChanged -= PowerManager_EffectivePowerModeChanged;
            PowerManager.UserPresenceStatusChanged -= PowerManager_UserPresenceStatusChanged;
            PowerManager.SystemSuspendStatusChanged -= PowerManager_SystemSuspendStatusChanged;
        }

        #endregion


        #region State/power notification callbacks 

        private static void PowerManager_BatteryStatusChanged(object sender, object e)
        {
            BatteryStatus batteryStatus = PowerManager.BatteryStatus;
            int remainingCharge = PowerManager.RemainingChargePercent;
            OutputMessage($"Battery status changed: {batteryStatus}, {remainingCharge}");
            DetermineWorkloads();
        }

        private static void PowerManager_PowerSupplyStatusChanged(object sender, object e)
        {
            PowerSupplyStatus powerStatus = PowerManager.PowerSupplyStatus;
            OutputMessage($"Power supply status changed: {powerStatus}");
            DetermineWorkloads();
        }

        private static void PowerManager_PowerSourceKindChanged(object sender, object e)
        {
            PowerSourceKind powerSource = PowerManager.PowerSourceKind;
            OutputMessage($"Power source kind changed: {powerSource}");
            DetermineWorkloads();
        }

        private static void PowerManager_RemainingChargePercentChanged(object sender, object e)
        {
            int remainingCharge = PowerManager.RemainingChargePercent;
            OutputMessage($"Remaining charge percent changed: {remainingCharge}");
            DetermineWorkloads();
        }

        private static void PowerManager_RemainingDischargeTimeChanged(object sender, object e)
        {
            TimeSpan remainingDischargeTime = PowerManager.RemainingDischargeTime;
            OutputMessage($"Remaining discharge time changed {remainingDischargeTime}");
            DetermineWorkloads();
        }

        private static void PowerManager_DisplayStatusChanged(object sender, object e)
        {
            DisplayStatus displayStatus = PowerManager.DisplayStatus;
            OutputMessage($"Display status changed: {displayStatus}");
            if (displayStatus == DisplayStatus.Off)
            {
                // The screen is off, let's stop rendering foreground graphics,
                // and instead kick off some background work now.
                StopUpdatingGraphics();
                StartDoingBackgroundWork();
            }
        }

        private static void PowerManager_EnergySaverStatusChanged(object sender, object e)
        {
            EnergySaverStatus energyStatus = PowerManager.EnergySaverStatus;
            OutputMessage($"Energy saver status changed: {energyStatus}");
            DetermineWorkloads();
        }

        private static async void PowerManager_EffectivePowerModeChanged(object sender, object e)
        {
            EffectivePowerMode powerMode = await PowerManager.EffectivePowerMode;
            OutputMessage($"Effective power mode changed: {powerMode}");
            DetermineWorkloads();
        }

        private static void PowerManager_UserPresenceStatusChanged(object sender, object e)
        {
            UserPresenceStatus userStatus = PowerManager.UserPresenceStatus;
            OutputMessage($"User presence status changed: {userStatus}");
            DetermineWorkloads();
        }

        private static void PowerManager_SystemSuspendStatusChanged(object sender, object e)
        {
            SystemSuspendStatus systemSuspendStatus = PowerManager.SystemSuspendStatus;
            OutputMessage($"System suspend status changed: {systemSuspendStatus}");
            DetermineWorkloads();
        }

        private static void DetermineWorkloads()
        {
            BatteryStatus batteryStatus = PowerManager.BatteryStatus;
            int remainingCharge = PowerManager.RemainingChargePercent;
            PowerSupplyStatus powerStatus = PowerManager.PowerSupplyStatus;
            PowerSourceKind powerSource = PowerManager.PowerSourceKind;

            if ((powerSource == PowerSourceKind.DC
                && batteryStatus == BatteryStatus.Discharging
                && remainingCharge < 25)
                || (powerSource == PowerSourceKind.AC
                && powerStatus == PowerSupplyStatus.Inadequate))
            {
                // The device is not in a good battery/power state, 
                // so we should pause any non-critical work.
                PauseNonCriticalWork();
            }
            else if ((batteryStatus != BatteryStatus.Discharging && remainingCharge > 75)
                && powerStatus != PowerSupplyStatus.Inadequate)
            {
                // The device is in good battery/power state,
                // so let's kick of some high-power work.
                StartPowerIntensiveWork();
            }
        }

        #endregion


        #region Simulate starting/stopping work 

        private static void PauseNonCriticalWork()
        {
            if (isWorkInProgress)
            {
                isWorkInProgress = false;
                OutputMessage("paused non-critical work");
            }
        }

        private static void StartPowerIntensiveWork()
        {
            if (!isWorkInProgress)
            {
                isWorkInProgress = true;
                OutputMessage("starting power-intensive work");
            }
        }

        private static void StopUpdatingGraphics()
        {
            OutputMessage("stopped updating graphics");
        }

        private static void StartDoingBackgroundWork()
        {
            OutputMessage("starting background work");
        }

        #endregion

    }
}
