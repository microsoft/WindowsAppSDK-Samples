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
    public sealed partial class Scenario1_MagnetometerDataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Magnetometer _magnetometer;
        private uint _desiredReportInterval;

        public Scenario1_MagnetometerDataEvents()
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
                    _magnetometer.ReadingChanged += ReadingChanged;
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _magnetometer.ReadingChanged -= ReadingChanged;
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        private void ReadingChanged(object sender, MagnetometerReadingChangedEventArgs e)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                MagnetometerReading reading = e.Reading;
                ScenarioOutput_X.Text = string.Format("{0,5:0.00}", reading.MagneticFieldX);
                ScenarioOutput_Y.Text = string.Format("{0,5:0.00}", reading.MagneticFieldY);
                ScenarioOutput_Z.Text = string.Format("{0,5:0.00}", reading.MagneticFieldZ);
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_magnetometer != null)
            {
                // Establish the report interval
                _magnetometer.ReportInterval = _desiredReportInterval;

                App.MainWindow.VisibilityChanged += VisibilityChanged;
                _magnetometer.ReadingChanged += ReadingChanged;

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
            _magnetometer.ReadingChanged -= ReadingChanged;

            // Restore the default report interval to release resources while the sensor is not in use
            _magnetometer.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
