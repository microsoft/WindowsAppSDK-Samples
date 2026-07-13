using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "Device Enumeration and Pairing C# sample";

        // The code for each scenario lives under Scenarios\DeviceEnumerationAndPairing\.
        // Note: the original UWP Scenario 3 (background-task device watcher) is intentionally
        // omitted; background tasks are tracked separately for the WinUI migration.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Device Enumeration and Pairing", new()
            {
                new Scenario { Title = "Device Picker Common Control", ClassType = typeof(Scenario1_DevicePicker) },
                new Scenario { Title = "Enumerate and Watch Devices", ClassType = typeof(Scenario2_DeviceWatcher) },
                new Scenario { Title = "Enumerate Snapshot of Devices", ClassType = typeof(Scenario4_Snapshot) },
                new Scenario { Title = "Get Single Device", ClassType = typeof(Scenario5_GetSingleDevice) },
                new Scenario { Title = "Custom Filter with Additional Properties", ClassType = typeof(Scenario6_CustomFilterAddedProps) },
                new Scenario { Title = "Request Specific DeviceInformationKind", ClassType = typeof(Scenario7_DeviceInformationKind) },
                new Scenario { Title = "Basic Device Pairing", ClassType = typeof(Scenario8_PairDevice) },
                new Scenario { Title = "Custom Device Pairing", ClassType = typeof(Scenario9_CustomPairDevice) },
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