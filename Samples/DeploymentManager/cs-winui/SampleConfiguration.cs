// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace DeploymentManagerSample
{
    internal static class SampleConfig
    {
        public const string FeatureName = "DeploymentManager";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Get Status", ClassName = typeof(Scenario1_GetStatus).FullName },
            new Scenario() { Title = "Initialize", ClassName = typeof(Scenario2_Initialize).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
