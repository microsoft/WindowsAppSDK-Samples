using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "AudioCategory C# sample";

        // The sample is a single page under Scenarios\AudioCategory\. The user picks an audio category
        // from a dropdown, plays a file, and can launch a second instance to observe how two categories
        // interact (ducking, muting, routing) in the system mixer.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Audio Category", new()
            {
                new Scenario { Title = "Audio categories", ClassType = typeof(Scenario1) },
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
