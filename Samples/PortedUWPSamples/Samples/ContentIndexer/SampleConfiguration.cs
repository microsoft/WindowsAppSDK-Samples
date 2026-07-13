using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "Content Indexer C# sample";

        // The code for each scenario lives under Scenarios\ContentIndexer\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Content Indexer", new()
            {
                new Scenario { Title = "Add item to the index using the ContentIndexer", ClassType = typeof(S1_AddWithAPI) },
                new Scenario { Title = "Update and delete indexed items using the ContentIndexer", ClassType = typeof(S2_UpdateAndDeleteWithAPI) },
                new Scenario { Title = "Retrieve indexed items added using the ContentIndexer", ClassType = typeof(S3_RetrieveWithAPI) },
                new Scenario { Title = "Check the index revision number", ClassType = typeof(S4_CheckIndexRevision) },
                new Scenario { Title = "Add indexed items by using appcontent-ms files", ClassType = typeof(S5_AddWithAppContent) },
                new Scenario { Title = "Delete indexed appcontent-ms files", ClassType = typeof(S6_DeleteWithAppContent) },
                new Scenario { Title = "Retrieve indexed properties from appcontent-ms files", ClassType = typeof(S7_RetrieveWithAppContent) },
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