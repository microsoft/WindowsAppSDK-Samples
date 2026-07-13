using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "AudioCreation";

        // The code for each scenario lives under Scenarios\AudioCreation\.
        // Scenario6 (Custom Effects) is omitted: it depends on a separate UWP Windows
        // Runtime Component (CustomEffect) that the AudioGraph activates by class name,
        // which would need to be re-authored as a C#/WinRT component.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("AudioCreation", new()
            {
                new Scenario { Title = "File Playback", ClassType = typeof(Scenario1_FilePlayback) },
                new Scenario { Title = "Capture From Device", ClassType = typeof(Scenario2_DeviceCapture) },
                new Scenario { Title = "Using A FrameInput Node", ClassType = typeof(Scenario3_FrameInput) },
                new Scenario { Title = "Using A Submix Node", ClassType = typeof(Scenario4_Submix) },
                new Scenario { Title = "Inbox Effects", ClassType = typeof(Scenario5_InboxEffects) },
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
