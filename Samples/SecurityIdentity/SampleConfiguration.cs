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
        public const string FEATURE_NAME = "Security and Identity C# sample";

        // Scenarios are grouped by security/identity feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("User info", new()
            {
                new Scenario { Title = "Find users", ClassType = typeof(Scenario1_FindUsers) },
                new Scenario { Title = "Watch users", ClassType = typeof(Scenario2_WatchUsers) },
                new Scenario { Title = "Check user consent group", ClassType = typeof(Scenario3_CheckUserConsentGroup) },
            }),
            new FeatureGroup("Key credential manager", new()
            {
                new Scenario { Title = "Windows Hello sign-in", ClassType = typeof(UserSelect) },
            }),
            new FeatureGroup("Personal data encryption", new()
            {
                new Scenario { Title = "Files and folders", ClassType = typeof(Scenario1_Files) },
                new Scenario { Title = "Memory", ClassType = typeof(Scenario2_Memory) },
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
