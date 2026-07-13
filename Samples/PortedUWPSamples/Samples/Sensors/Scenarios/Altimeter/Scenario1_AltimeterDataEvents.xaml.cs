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
    public sealed partial class Scenario1_AltimeterDataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Altimeter _altimeter;
        private uint _desiredReportIntervalMs;

        public Scenario1_AltimeterDataEvents()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _altimeter = Altimeter.GetDefault();
            if (_altimeter != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                _desiredReportIntervalMs = Math.Max(_altimeter.MinimumReportInterval, 1000);

                ScenarioEnableButton.IsEnabled = true;
                ScenarioDisableButton.IsEnabled = false;
            }
            else
            {
                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = false;
                rootPage.NotifyUser("No altimeter found", NotifyType.ErrorMessage);
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
                    _altimeter.ReadingChanged += ReadingChanged;
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _altimeter.ReadingChanged -= ReadingChanged;
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        private void ReadingChanged(object sender, AltimeterReadingChangedEventArgs e)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                AltimeterReading reading = e.Reading;
                ScenarioOutput_M.Text = string.Format("{0,5:0.00}", reading.AltitudeChangeInMeters);
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_altimeter != null)
            {
                // Establish the report interval
                _altimeter.ReportInterval = _desiredReportIntervalMs;

                App.MainWindow.VisibilityChanged += VisibilityChanged;
                _altimeter.ReadingChanged += ReadingChanged;

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No altimeter found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            App.MainWindow.VisibilityChanged -= VisibilityChanged;
            _altimeter.ReadingChanged -= ReadingChanged;

            // Restore the default report interval to release resources while the sensor is not in use
            _altimeter.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
