using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "PlayReady C# sample";

        // The code for each scenario lives under Scenarios\PlayReady\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("PlayReady", new()
            {
                new Scenario { Title = "Reactive License Request", ClassType = typeof(Scenario1_ReactiveRequest) },
                new Scenario { Title = "Proactive License Request", ClassType = typeof(Scenario2_ProactiveRequest) },
                new Scenario { Title = "Manage HW/SW DRM", ClassType = typeof(Scenario3_HardwareDRM) },
                new Scenario { Title = "Secure Stop", ClassType = typeof(Scenario4_SecureStop) },
            }),
        };

        // A protected H.264 DASH (OnDemand) asset used across the scenarios.
        public static string SampleMovieURL = "http://profficialsite.origin.mediaservices.windows.net/c51358ea-9a5e-4322-8951-897d640fdfd7/tearsofsteel_4k.ism/manifest(format=mpd-time-csf)";
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
