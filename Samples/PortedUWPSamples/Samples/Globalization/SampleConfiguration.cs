//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "Globalization C# sample";

        // Scenarios are grouped by globalization feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Number formatting", new()
            {                new Scenario { Title = "Percent and permille formatting", ClassType = typeof(Scenario1_PercentPermilleFormatting) },
                new Scenario { Title = "Decimal formatting", ClassType = typeof(Scenario2_DecimalFormatting) },
                new Scenario { Title = "Currency formatting", ClassType = typeof(Scenario3_CurrencyFormatting) },
                new Scenario { Title = "Number parsing", ClassType = typeof(Scenario4_NumberParsing) },
                new Scenario { Title = "Rounding and padding", ClassType = typeof(Scenario5_RoundingAndPadding) },
                new Scenario { Title = "Numeral system translation", ClassType = typeof(Scenario6_NumeralSystemTranslation) },
                new Scenario { Title = "Formatting/translation using Unicode extensions", ClassType = typeof(Scenario7_UsingUnicodeExtensions) },
            }),
            new FeatureGroup("Unicode tokenization", new()
            {
                new Scenario { Title = "Tokenize lexical identifiers within a string", ClassType = typeof(Scenario1_FindId) },
            }),
            new FeatureGroup("Text segmentation", new()
            {
                new Scenario { Title = "Extract text segments", ClassType = typeof(Scenario1_ExtractTextSegments) },
                new Scenario { Title = "Get current text segment from index", ClassType = typeof(Scenario2_GetCurrentTextSegmentFromIndex) },
            }),
            new FeatureGroup("Language font", new()
            {
                new Scenario { Title = "Fonts for UI", ClassType = typeof(Scenario1_UIFonts) },
                new Scenario { Title = "Fonts for documents", ClassType = typeof(Scenario2_DocumentFonts) },
            }),
            new FeatureGroup("Text suggestion", new()
            {
                new Scenario { Title = "Conversion", ClassType = typeof(Scenario1_Conversion) },
                new Scenario { Title = "Prediction", ClassType = typeof(Scenario2_Prediction) },
                new Scenario { Title = "Reverse conversion", ClassType = typeof(Scenario3_ReverseConversion) },
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
