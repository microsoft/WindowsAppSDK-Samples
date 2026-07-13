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

using System.Collections.Generic;
using Windows.Devices.Sensors;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario0_AccelerometerChoose : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario0_AccelerometerChoose()
        {
            this.InitializeComponent();
            var readingTypes = new List<AccelerometerReadingType>()
            {
                AccelerometerReadingType.Standard,
                AccelerometerReadingType.Linear,
                AccelerometerReadingType.Gravity,
            };
            ReadingTypeComboBox.ItemsSource = readingTypes;
            ReadingTypeComboBox.SelectedIndex = readingTypes.IndexOf(AccelerometerHelper.ReadingType);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            AccelerometerHelper.ReadingType = (AccelerometerReadingType)ReadingTypeComboBox.SelectedValue;
        }
    }
}
