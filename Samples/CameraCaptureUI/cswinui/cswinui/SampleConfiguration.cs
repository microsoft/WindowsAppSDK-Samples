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
            new Scenario() { Title = "Capture Photo", ClassName = typeof(Scenario1_CapturePhoto).FullName },
            new Scenario() { Title = "Capture Video", ClassName = typeof(Scenario2_CaptureVideo).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
