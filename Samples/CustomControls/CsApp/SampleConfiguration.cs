// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace CsApp
{
    internal static class SampleConfig
    {
        public const string FeatureName = "CsApp";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Custom Control", ClassName = typeof(Scenario1_CustomControl).FullName },
            new Scenario() { Title = "User Control", ClassName = typeof(Scenario2_UserControl).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
