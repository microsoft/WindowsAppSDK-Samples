using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "MobileNetworking";

        // Consolidated sample: MobileBroadband + MobileHotspot.
        // Code lives under Scenarios\MobileBroadband\ and Scenarios\MobileHotspot\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Mobile Broadband", new()
            {
                new Scenario { Title = "Mobile Broadband Device", ClassType = typeof(BroadbandDevice) },
                new Scenario { Title = "Network Account Watcher", ClassType = typeof(AccountWatcher) },
                new Scenario { Title = "Connection UI", ClassType = typeof(ShowConnection) },
                new Scenario { Title = "Get Connection Profiles", ClassType = typeof(GetConnectionProfiles) },
                new Scenario { Title = "Modem Information", ClassType = typeof(ModemInformation) },
                new Scenario { Title = "Device Services", ClassType = typeof(ModemDeviceServices) },
                new Scenario { Title = "SIM Card", ClassType = typeof(SimCard) },
            }),
            new FeatureGroup("Mobile Hotspot", new()
            {
                new Scenario { Title = "Configure Mobile Hotspot", ClassType = typeof(Scenario1_ConfigureMobileHotspot) },
                new Scenario { Title = "Toggle Mobile Hotspot", ClassType = typeof(Scenario2_ToggleMobileHotspot) },
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
