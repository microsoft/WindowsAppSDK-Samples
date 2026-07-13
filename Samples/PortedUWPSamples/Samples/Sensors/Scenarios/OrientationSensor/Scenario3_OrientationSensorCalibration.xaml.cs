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

using Windows.Devices.Sensors;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_OrientationSensorCalibration : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        OrientationSensorCalibrationBar OrientationSensorCalibrationBar = new OrientationSensorCalibrationBar();

        public Scenario3_OrientationSensorCalibration()
        {
            this.InitializeComponent();

            if (OrientationSensorHelper.SensorReadingType == SensorReadingType.Relative)
            {
                EnabledContent.Visibility = Visibility.Collapsed;
                DisabledContent.Visibility = Visibility.Visible;
            }
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            OrientationSensorCalibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for high accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        private void OnHighAccuracy()
        {
            OrientationSensorCalibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for approximate accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        private void OnApproximateAccuracy()
        {
            OrientationSensorCalibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for unreliable accuracy. Sensor does not meet accuracy
        /// requirements. Request to show the calibration bar per the calibration guidance.
        /// </summary>
        private void OnUnreliableAccuracy()
        {
            OrientationSensorCalibrationBar.RequestCalibration(MagnetometerAccuracy.Unreliable);
        }
    }
}
