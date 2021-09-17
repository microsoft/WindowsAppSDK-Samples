﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;
using Microsoft.Windows.System.Power;

namespace CsWinFormsState
{
    public partial class MainForm : Form
    {

        #region Init

        private bool isWorkInProgress;

        public MainForm()
        {
            InitializeComponent();
        }

        private void OutputMessage(string message)
        {
            if (!listView1.InvokeRequired)
            {
                listView1.Items.Add(message);
            }
            else
            {
                listView1.Invoke((Action)(delegate { listView1.Items.Add(message); }));
            }
        }

        #endregion


        #region Register/Unregister callbacks

        private void RegisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Registering for state notifications");
            PowerManager.BatteryStatusChanged += PowerManager_BatteryStatusChanged;
            PowerManager.PowerSupplyStatusChanged += PowerManager_PowerSupplyStatusChanged;
            PowerManager.PowerSourceKindChanged += PowerManager_PowerSourceKindChanged;
            PowerManager.RemainingChargePercentChanged += PowerManager_RemainingChargePercentChanged;
            PowerManager.RemainingDischargeTimeChanged += PowerManager_RemainingDischargeTimeChanged;
            PowerManager.DisplayStatusChanged += PowerManager_DisplayStatusChanged;
        }

        private void UnregisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Unregistering state notifications");
            PowerManager.BatteryStatusChanged -= PowerManager_BatteryStatusChanged;
            PowerManager.PowerSupplyStatusChanged -= PowerManager_PowerSupplyStatusChanged;
            PowerManager.PowerSourceKindChanged -= PowerManager_PowerSourceKindChanged;
            PowerManager.RemainingChargePercentChanged -= PowerManager_RemainingChargePercentChanged;
            PowerManager.RemainingDischargeTimeChanged -= PowerManager_RemainingDischargeTimeChanged;
            PowerManager.DisplayStatusChanged -= PowerManager_DisplayStatusChanged;
        }

        #endregion


        #region State/power notification callbacks 

        private void PowerManager_BatteryStatusChanged(object sender, object e)
        {
            BatteryStatus batteryStatus = PowerManager.BatteryStatus;
            int remainingCharge = PowerManager.RemainingChargePercent;
            OutputMessage($"Battery status changed: {batteryStatus}, {remainingCharge}");
            DetermineWorkloads();
        }

        private void PowerManager_PowerSupplyStatusChanged(object sender, object e)
        {
            PowerSupplyStatus powerStatus = PowerManager.PowerSupplyStatus;
            OutputMessage($"Power supply status changed: {powerStatus}");
            DetermineWorkloads();
        }

        private void PowerManager_PowerSourceKindChanged(object sender, object e)
        {
            PowerSourceKind powerSource = PowerManager.PowerSourceKind;
            OutputMessage($"Power source kind changed: {powerSource}");
            DetermineWorkloads();
        }

        private void PowerManager_RemainingChargePercentChanged(object sender, object e)
        {
            int remainingCharge = PowerManager.RemainingChargePercent;
            OutputMessage($"Remaining charge percent changed: {remainingCharge}");
            DetermineWorkloads();
        }

        private void PowerManager_RemainingDischargeTimeChanged(object sender, object e)
        {
            TimeSpan remainingDischargeTime = PowerManager.RemainingDischargeTime;
            OutputMessage($"Remaining discharge time changed {remainingDischargeTime}");
            DetermineWorkloads();
        }

        private void PowerManager_DisplayStatusChanged(object sender, object e)
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

        private void DetermineWorkloads()
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

        private void PauseNonCriticalWork()
        {
            if (isWorkInProgress)
            {
                isWorkInProgress = false;
                OutputMessage("paused non-critical work");
            }
        }

        private void StartPowerIntensiveWork()
        {
            if (!isWorkInProgress)
            {
                isWorkInProgress = true;
                OutputMessage("starting power-intensive work");
            }
        }

        private void StopUpdatingGraphics()
        {
            OutputMessage("stopped updating graphics");
        }

        private void StartDoingBackgroundWork()
        {
            OutputMessage("starting background work");
        }

        #endregion

    }
}
