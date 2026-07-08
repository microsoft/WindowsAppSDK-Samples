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
        public const string FEATURE_NAME = "Lighting C# sample";

        // Scenarios are grouped by lighting feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Lamp array", new()
            {
                new Scenario { Title = "LampArray basics", ClassType = typeof(Scenario1_Basics) },
                new Scenario { Title = "LampArray effects", ClassType = typeof(Scenario2_Effects) },
            }),
            new FeatureGroup("Lamp device", new()
            {
                new Scenario { Title = "Get lamp instance", ClassType = typeof(Scenario1_GetLamp) },
                new Scenario { Title = "Enable lamp and settings adjustment", ClassType = typeof(Scenario2_EnableSettings) },
                new Scenario { Title = "Lamp device change events", ClassType = typeof(Scenario3_AvailabilityChanged) },
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
