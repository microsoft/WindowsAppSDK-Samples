using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "AudioCategory C# sample";

        // The code for each scenario lives under Scenarios\AudioCategory\.
        // Each scenario plays an audio stream tagged with a different audio category so the
        // system mixer treats it appropriately (ducking, muting, routing).
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Audio Category", new()
            {
                new Scenario { Title = "Movie", ClassType = typeof(Scenario1) },
                new Scenario { Title = "Media", ClassType = typeof(Scenario2) },
                new Scenario { Title = "Game Chat", ClassType = typeof(Scenario3) },
                new Scenario { Title = "Speech", ClassType = typeof(Scenario4) },
                new Scenario { Title = "Communications", ClassType = typeof(Scenario5) },
                new Scenario { Title = "Alerts", ClassType = typeof(Scenario6) },
                new Scenario { Title = "Sound Effects", ClassType = typeof(Scenario7) },
                new Scenario { Title = "Game Effects", ClassType = typeof(Scenario8) },
                new Scenario { Title = "Game Media", ClassType = typeof(Scenario9) },
                new Scenario { Title = "Other", ClassType = typeof(Scenario10) },
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
