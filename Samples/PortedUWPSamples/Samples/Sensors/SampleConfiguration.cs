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

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "Sensors C# sample";

        // Scenarios are grouped by sensor. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each sensor lives under Scenarios\<Sensor>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Accelerometer", new()
            {
                new Scenario { Title = "Choose accelerometer", ClassType = typeof(Scenario0_AccelerometerChoose) },
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_AccelerometerDataEvents) },
                new Scenario { Title = "Shake events", ClassType = typeof(Scenario2_AccelerometerShakeEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario3_AccelerometerPolling) },
                new Scenario { Title = "Data events batching", ClassType = typeof(Scenario5_AccelerometerDataEventsBatching) },
            }),
            new FeatureGroup("Gyrometer", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_GyrometerDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_GyrometerPolling) },
                new Scenario { Title = "Cross-platform porting", ClassType = typeof(Scenario3_GyrometerPorting) },
            }),
            new FeatureGroup("Inclinometer", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_InclinometerDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_InclinometerPolling) },
                new Scenario { Title = "Calibration", ClassType = typeof(Scenario3_InclinometerCalibration) },
            }),
            new FeatureGroup("Relative inclinometer", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_RelativeInclinometerDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_RelativeInclinometerPolling) },
            }),
            new FeatureGroup("Orientation sensor", new()
            {
                new Scenario { Title = "Choose orientation sensor", ClassType = typeof(Scenario0_OrientationSensorChoose) },
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_OrientationSensorDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_OrientationSensorPolling) },
                new Scenario { Title = "Calibration", ClassType = typeof(Scenario3_OrientationSensorCalibration) },
            }),
            new FeatureGroup("Simple orientation sensor", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_SimpleOrientationSensorDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_SimpleOrientationSensorPolling) },
            }),
            new FeatureGroup("Compass", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_CompassDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_CompassPolling) },
                new Scenario { Title = "Calibration", ClassType = typeof(Scenario3_CompassCalibration) },
            }),
            new FeatureGroup("Magnetometer", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_MagnetometerDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_MagnetometerPolling) },
            }),
            new FeatureGroup("Altimeter", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_AltimeterDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_AltimeterPolling) },
            }),
            new FeatureGroup("Barometer", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_BarometerDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_BarometerPolling) },
            }),
            new FeatureGroup("Light sensor", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_LightSensorDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_LightSensorPolling) },
            }),
            new FeatureGroup("Presence sensor", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_PresenceSensorDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_PresenceSensorPolling) },
                new Scenario { Title = "Choosing", ClassType = typeof(Scenario3_PresenceSensorChoosing) },
            }),
            new FeatureGroup("Activity sensor", new()
            {
                new Scenario { Title = "Current activity", ClassType = typeof(Scenario1_ActivitySensorCurrentActivity) },
                new Scenario { Title = "Activity history", ClassType = typeof(Scenario2_ActivitySensorHistory) },
                new Scenario { Title = "Events", ClassType = typeof(Scenario3_ActivitySensorChangeEvents) },
            }),
            new FeatureGroup("Pedometer", new()
            {
                new Scenario { Title = "Events", ClassType = typeof(Scenario1_PedometerEvents) },
                new Scenario { Title = "History", ClassType = typeof(Scenario2_PedometerHistory) },
                new Scenario { Title = "Current step count", ClassType = typeof(Scenario3_PedometerCurrentStepCount) },
            }),
            new FeatureGroup("Proximity sensor", new()
            {
                new Scenario { Title = "Data events", ClassType = typeof(Scenario1_ProximitySensorDataEvents) },
                new Scenario { Title = "Polling", ClassType = typeof(Scenario2_ProximitySensorPolling) },
                new Scenario { Title = "Display on/off", ClassType = typeof(Scenario3_ProximitySensorDisplayOnOff) },
            }),
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public class FeatureGroup
    {
        public FeatureGroup(string title, List<Scenario> scenarios)
        {
            Title = title;
            Scenarios = scenarios;
        }

        public string Title { get; set; }
        public List<Scenario> Scenarios { get; set; }
    }
}
