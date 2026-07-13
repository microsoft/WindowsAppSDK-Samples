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

namespace SDKTemplate
{
    public sealed partial class Scenario2_AltimeterPolling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Altimeter _altimeter;

        public Scenario2_AltimeterPolling()
        {
            this.InitializeComponent();

            _altimeter = Altimeter.GetDefault();
            if (_altimeter == null)
            {
                rootPage.NotifyUser("No altimeter found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'GetData' button.
        /// </summary>
        private void ScenarioGetData(object sender, RoutedEventArgs e)
        {
            if (_altimeter != null)
            {
                AltimeterReading reading = _altimeter.GetCurrentReading();
                if (reading != null)
                {
                    ScenarioOutput_M.Text = string.Format("{0,5:0.00}", reading.AltitudeChangeInMeters);
                }
            }
            else
            {
                rootPage.NotifyUser("No altimeter found", NotifyType.ErrorMessage);
            }
        }
    }
}
