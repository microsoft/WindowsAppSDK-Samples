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

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using Windows.Devices.Sensors;
using Windows.Foundation;
using System.Threading.Tasks;

namespace SDKTemplate
{
    public sealed partial class Scenario3_InclinometerCalibration : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        InclinometerCalibrationBar InclinometerCalibrationBar = new InclinometerCalibrationBar();

        public Scenario3_InclinometerCalibration()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            InclinometerCalibrationBar.Hide();
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the click handler for high accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnHighAccuracy(object sender, RoutedEventArgs e)
        {
            InclinometerCalibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for approximate accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnApproximateAccuracy(object sender, RoutedEventArgs e)
        {
            InclinometerCalibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for unreliable accuracy. Sensor does not meet accuracy
        /// requirements. Request to show the calibration bar per the calibration guidance.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnUnreliableAccuracy(object sender, RoutedEventArgs e)
        {
            InclinometerCalibrationBar.RequestCalibration(MagnetometerAccuracy.Unreliable);
        }
    }
}


