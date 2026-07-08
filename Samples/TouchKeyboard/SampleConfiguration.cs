using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "TouchKeyboard";

        // The code for each scenario lives under Scenarios\TouchKeyboard\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("TouchKeyboard", new()
            {
                new Scenario { Title = "Display touch keyboard automatically", ClassType = typeof(Scenario1_Launch) },
                new Scenario { Title = "Listen for Show/Hide events", ClassType = typeof(Scenario2_ShowHideEvents) },
                new Scenario { Title = "Programmatically Show/Hide", ClassType = typeof(Scenario3_ShowHideMethods) },
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
