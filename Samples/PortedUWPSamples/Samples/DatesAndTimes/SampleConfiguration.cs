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
        public const string FEATURE_NAME = "DatesAndTimes C# sample";

        // Scenarios are grouped by feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Date and time formatting", new()
            {
                new Scenario { Title = "Long and short formats", ClassType = typeof(Scenario1_LongAndShortFormats) },
                new Scenario { Title = "Format via string template", ClassType = typeof(Scenario2_StringTemplate) },
                new Scenario { Title = "Format via parameterized template", ClassType = typeof(Scenario3_ParameterizedTemplate) },
                new Scenario { Title = "Override the user's global context", ClassType = typeof(Scenario4_Override) },
                new Scenario { Title = "Unicode extensions", ClassType = typeof(Scenario5_UnicodeExtensions) },
                new Scenario { Title = "Time zones", ClassType = typeof(Scenario6_TimeZone) },
            }),
            new FeatureGroup("Calendar", new()
            {
                new Scenario { Title = "Calendar data", ClassType = typeof(Scenario1_Data) },
                new Scenario { Title = "Calendar statistics", ClassType = typeof(Scenario2_Stats) },
                new Scenario { Title = "Enumeration and math", ClassType = typeof(Scenario3_Enum) },
                new Scenario { Title = "Unicode extensions", ClassType = typeof(Scenario4_UnicodeExtensions) },
                new Scenario { Title = "Time zone support", ClassType = typeof(Scenario5_TimeZone) },
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
