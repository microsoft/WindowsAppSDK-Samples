using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "TouchKeyboardTextInput";

        // The code for each scenario lives under Scenarios\TouchKeyboardTextInput\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("TouchKeyboardTextInput", new()
            {
                new Scenario { Title = "Spelling and Text Suggestions", ClassType = typeof(Scenario1_SpellingTextSuggestions) },
                new Scenario { Title = "Scoped Views", ClassType = typeof(Scenario2_ScopedViews) },
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
