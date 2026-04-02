// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace cswinui
{
    internal static class SampleConfig
    {
        public const string FeatureName = "cswinui";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Numeric Badge", ClassName = typeof(Scenario1_NumericBadge).FullName },
            new Scenario() { Title = "Glyph Badge", ClassName = typeof(Scenario2_GlyphBadge).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
