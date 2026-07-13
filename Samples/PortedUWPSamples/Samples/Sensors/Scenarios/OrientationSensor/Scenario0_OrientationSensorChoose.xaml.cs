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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario0_OrientationSensorChoose : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario0_OrientationSensorChoose()
        {
            this.InitializeComponent();
            var readingTypes = new List<SensorReadingType>()
            {
                SensorReadingType.Absolute,
                SensorReadingType.Relative,
            };
            ReadingTypeComboBox.ItemsSource = readingTypes;
            ReadingTypeComboBox.SelectedIndex = readingTypes.IndexOf(OrientationSensorHelper.SensorReadingType);

            var optimizationGoals = new List<SensorOptimizationGoal>()
            {
                SensorOptimizationGoal.Precision,
                SensorOptimizationGoal.PowerEfficiency,
            };
            OptimizationGoalComboBox.ItemsSource = optimizationGoals;
            OptimizationGoalComboBox.SelectedIndex = optimizationGoals.IndexOf(OrientationSensorHelper.SensorOptimizationGoal);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            OrientationSensorHelper.SensorReadingType = (SensorReadingType)ReadingTypeComboBox.SelectedValue;
            OrientationSensorHelper.SensorOptimizationGoal = (SensorOptimizationGoal)OptimizationGoalComboBox.SelectedValue;
        }
    }
}
