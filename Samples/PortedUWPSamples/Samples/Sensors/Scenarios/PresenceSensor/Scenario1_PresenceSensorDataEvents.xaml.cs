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
    public sealed partial class Scenario1_PresenceSensorDataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private HumanPresenceSensor sensor;

        public Scenario1_PresenceSensorDataEvents()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (sensor != null)
            {
                sensor.ReadingChanged -= ReadingChanged;
                sensor = null;
            }
        }

        private async void ScenarioGetSensor_Click(object sender, RoutedEventArgs e)
        {
            // Disable the button so the user can't click it again while we are already working.
            GetSensorButton.IsEnabled = false;

            // Clear any previous message.
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Try to get the default presence sensor.
            try
            {
                sensor = await HumanPresenceSensor.GetDefaultAsync();
            }
            catch (UnauthorizedAccessException)
            {
                // The user denied this app presence sensor access.
                sensor = null;
            }
            GetSensorButton.IsEnabled = true;

            if (sensor != null)
            {
                VisualStateManager.GoToState(this, "Ready", true);

                // Begin listening for data reports.
                sensor.ReadingChanged += ReadingChanged;
            }
            else
            {
                rootPage.NotifyUser("No presence sensor found", NotifyType.ErrorMessage);
            }
        }

        private void ReadingChanged(HumanPresenceSensor sender, HumanPresenceSensorReadingChangedEventArgs e)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                HumanPresenceSensorReading reading = e.Reading;
                if (null != reading)
                {
                    if (sensor.IsEngagementSupported)
                    {
                        EngagementStateTextBlock.Text = reading.Engagement.ToString();
                    }
                    else
                    {
                        EngagementStateTextBlock.Text = "Not Supported";
                    }

                    if (sensor.IsPresenceSupported)
                    {
                        DetectionStateTextBlock.Text = reading.Presence.ToString();
                    }
                    else
                    {
                        DetectionStateTextBlock.Text = "Not Supported";
                    }

                    // Show the detection distance, if available
                    DetectionDistanceTextBlock.Text = reading.DistanceInMillimeters == null ? "Unavailable" : reading.DistanceInMillimeters.ToString();

                    TimestampTextBlock.Text = reading.Timestamp.ToString("u");
                }
            });
        }
    }
}
