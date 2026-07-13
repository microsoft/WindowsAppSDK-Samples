using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "PowerGrid C# sample";

        // The code for each scenario lives under Scenarios\PowerGrid\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Power Grid Forecast", new()
            {
                new Scenario { Title = "Find best time", ClassType = typeof(Scenario1_FindBest) },
                new Scenario { Title = "Display full forecast", ClassType = typeof(Scenario2_PrintFullForecast) },
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