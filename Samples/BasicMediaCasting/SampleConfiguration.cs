using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "BasicMediaCasting";

        // The code for each scenario lives under Scenarios\BasicMediaCasting\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("BasicMediaCasting", new()
            {
                new Scenario { Title = "Basic Media Casting", ClassType = typeof(Scenario1) },
                new Scenario { Title = "Casting Using Casting Picker", ClassType = typeof(Scenario2) },
                new Scenario { Title = "Casting Using Custom Picker", ClassType = typeof(Scenario3) },
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
