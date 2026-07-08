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
        public const string FEATURE_NAME = "x:DeferLoadStrategy C# sample";

        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("x:DeferLoadStrategy", new()
            {
                new Scenario { Title = "Basic Deferral", ClassType = typeof(BasicDeferral) },
                new Scenario { Title = "Adaptive Deferral", ClassType = typeof(AdaptivePage) },
                new Scenario { Title = "Control Template Deferral", ClassType = typeof(DeferredControlTemplatePart) },
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
