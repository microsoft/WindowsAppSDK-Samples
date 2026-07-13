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
    public sealed partial class Scenario1_BarometerDataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Barometer _barometer;
        private uint _desiredReportIntervalMs;

        public Scenario1_BarometerDataEvents()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _barometer = Barometer.GetDefault();
            if (_barometer != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                _desiredReportIntervalMs = Math.Max(_barometer.MinimumReportInterval, 1000);

                ScenarioEnableButton.IsEnabled = true;
                ScenarioDisableButton.IsEnabled = false;
            }
            else
            {
                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = false;
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage);
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
                    _barometer.ReadingChanged += ReadingChanged;
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _barometer.ReadingChanged -= ReadingChanged;
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        private void ReadingChanged(object sender, BarometerReadingChangedEventArgs e)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                BarometerReading reading = e.Reading;
                ScenarioOutput_hPa.Text = string.Format("{0,5:0.00}", reading.StationPressureInHectopascals);
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_barometer != null)
            {
                // Establish the report interval
                _barometer.ReportInterval = _desiredReportIntervalMs;

                App.MainWindow.VisibilityChanged += VisibilityChanged;
                _barometer.ReadingChanged += ReadingChanged;

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            App.MainWindow.VisibilityChanged -= VisibilityChanged;
            _barometer.ReadingChanged -= ReadingChanged;

            // Restore the default report interval to release resources while the sensor is not in use
            _barometer.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
