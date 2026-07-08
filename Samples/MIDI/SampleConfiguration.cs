using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "MIDI C# sample";

        // The code for each scenario lives under Scenarios\MIDI\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("MIDI", new()
            {
                new Scenario { Title = "MIDI Device Enumeration", ClassType = typeof(Scenario1_MIDIDeviceEnumeration) },
                new Scenario { Title = "Receive MIDI Messages", ClassType = typeof(Scenario2_ReceiveMIDIMessages) },
                new Scenario { Title = "Send MIDI Messages", ClassType = typeof(Scenario3_SendMIDIMessages) },
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