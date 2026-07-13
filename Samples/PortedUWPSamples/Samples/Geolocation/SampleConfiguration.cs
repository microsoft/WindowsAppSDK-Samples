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
        public const string FEATURE_NAME = "Geolocation C# sample";

        // The code for each scenario lives under Scenarios\Geolocation\.
        // Background-task scenarios (3, 5, 8) from the UWP sample are intentionally omitted
        // pending a WinUI 3 desktop background-activation design.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Geolocation", new()
            {
                new Scenario { Title = "Track position", ClassType = typeof(Scenario1_TrackPosition) },
                new Scenario { Title = "Get position", ClassType = typeof(Scenario2_GetPosition) },
                new Scenario { Title = "Foreground geofencing", ClassType = typeof(Scenario4_ForegroundGeofence) },
                new Scenario { Title = "Get last visit", ClassType = typeof(Scenario6_GetLastVisit) },
                new Scenario { Title = "Foreground visit monitoring", ClassType = typeof(Scenario7_ForegroundVisits) },
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
