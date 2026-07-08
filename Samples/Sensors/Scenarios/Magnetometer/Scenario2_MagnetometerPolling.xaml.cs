//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.Devices.Sensors;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_MagnetometerPolling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Magnetometer _magnetometer;
        private uint _desiredReportInterval;
        private DispatcherTimer _dispatcherTimer;

        public Scenario2_MagnetometerPolling()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _magnetometer = Magnetometer.GetDefault();
            if (_magnetometer != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                _desiredReportInterval = Math.Max(_magnetometer.MinimumReportInterval, 16);

                // Set up a DispatchTimer
                _dispatcherTimer = new DispatcherTimer();
                _dispatcherTimer.Tick += DisplayCurrentReading;
                _dispatcherTimer.Interval = TimeSpan.FromMilliseconds(_desiredReportInterval);

                ScenarioEnableButton.IsEnabled = true;
                ScenarioDisableButton.IsEnabled = false;
            }
            else
            {
                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = false;
                rootPage.NotifyUser("No magnetometer found", NotifyType.ErrorMessage);
            }
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                ScenarioDisable(null, null);
            }
        }

        /// <summary>
        /// This is the event handler for VisibilityChanged events. You would register for these notifications
        /// if handling sensor data when the app is not visible could cause unintended actions in the app.
        /// </summary>
        private void VisibilityChanged(object sender, WindowVisibilityChangedEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                if (e.Visible)
                {
                    // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
                    _dispatcherTimer.Start();
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _dispatcherTimer.Stop();
                }
            }
        }

        /// <summary>
        /// This is the dispatcher callback.
        /// </summary>
        private void DisplayCurrentReading(object sender, object args)
        {
            MagnetometerReading reading = _magnetometer.GetCurrentReading();
            if (reading != null)
            {
                ScenarioOutput_X.Text = string.Format("{0,5:0.00}", reading.MagneticFieldX);
                ScenarioOutput_Y.Text = string.Format("{0,5:0.00}", reading.MagneticFieldY);
                ScenarioOutput_Z.Text = string.Format("{0,5:0.00}", reading.MagneticFieldZ);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_magnetometer != null)
            {
                // Set the report interval to enable the sensor for polling
                _magnetometer.ReportInterval = _desiredReportInterval;

                App.MainWindow.VisibilityChanged += VisibilityChanged;
                _dispatcherTimer.Start();

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No magnetometer found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            App.MainWindow.VisibilityChanged -= VisibilityChanged;

            // Stop the dispatcher
            _dispatcherTimer.Stop();

            // Restore the default report interval to release resources while the sensor is not in use
            _magnetometer.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
