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
        public const string FEATURE_NAME = "DatagramSocket C# sample";

        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("DatagramSocket", new()
            {
                new Scenario { Title = "Start Datagram Listener", ClassType = typeof(Scenario1_Start) },
                new Scenario { Title = "Connect to Listener", ClassType = typeof(Scenario2_Connect) },
                new Scenario { Title = "Send Data", ClassType = typeof(Scenario3_Send) },
                new Scenario { Title = "Close Socket", ClassType = typeof(Scenario4_Close) },
                new Scenario { Title = "Multicast/Broadcast", ClassType = typeof(Scenario5_MulticastAndBroadcast) },
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
