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
        public const string FEATURE_NAME = "Simple imaging C# sample";

        // Each scenario becomes a NavigationView entry. Single-scenario groups render as
        // leaf items. The code for each scenario lives under Scenarios\SimpleImaging\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Image properties (FileProperties)", new()
            {
                new Scenario { Title = "Image properties (FileProperties)", ClassType = typeof(Scenario1) },
            }),
            new FeatureGroup("Image transforms/encode (BitmapDecoder)", new()
            {
                new Scenario { Title = "Image transforms/encode (BitmapDecoder)", ClassType = typeof(Scenario2) },
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
