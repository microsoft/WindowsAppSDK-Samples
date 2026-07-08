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
        public const string FEATURE_NAME = "NetworkConnectivity C# sample";

        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Network connectivity", new()
            {
                new Scenario { Title = "Query network connectivity", ClassType = typeof(Scenario1_NetworkConnectivity) },
                new Scenario { Title = "Get network cost information", ClassType = typeof(Scenario2_NetworkCost) },
                new Scenario { Title = "Listen to connectivity changes", ClassType = typeof(Scenario3_NetworkConnectivityChanges) },
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
