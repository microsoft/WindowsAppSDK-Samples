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
        public const string FEATURE_NAME = "Bluetooth C# sample";

        // Scenarios are grouped by Bluetooth feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Bluetooth Low Energy", new()
            {
                new Scenario { Title = "Client: Discover servers", ClassType = typeof(Scenario1_Discovery) },
                new Scenario { Title = "Client: Connect to a server", ClassType = typeof(Scenario2_Client) },
            }),
            new FeatureGroup("RFCOMM chat", new()
            {
                new Scenario { Title = "Chat client", ClassType = typeof(Scenario1_ChatClient) },
                new Scenario { Title = "Foreground chat server", ClassType = typeof(Scenario2_ChatServer) },
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
