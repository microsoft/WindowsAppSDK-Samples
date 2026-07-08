using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "RadioManager C# sample";

        // The code for each scenario lives under Scenarios\RadioManager\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Toggle Radios", new()
            {
                new Scenario { Title = "Toggle Radios", ClassType = typeof(Scenario1_Toggle) },
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
