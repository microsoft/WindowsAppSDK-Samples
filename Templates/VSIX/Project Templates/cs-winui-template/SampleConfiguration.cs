// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace $safeprojectname$
{
    internal static class SampleConfig
    {
        public const string FeatureName = "$safeprojectname$";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Scenario 1", ClassName = typeof(Scenario1_ShortName).FullName },
            new Scenario() { Title = "Scenario 2", ClassName = typeof(Scenario2_ShortName).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
