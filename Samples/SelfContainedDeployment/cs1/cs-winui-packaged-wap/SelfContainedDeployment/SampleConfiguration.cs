// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace SelfContainedDeployment
{
    internal static class SampleConfig
    {
        public const string FeatureName = "Self-Contained Deployment";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Self-Contained Deployment", ClassName = typeof(Scenario1_SelfContainedDeployment).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
