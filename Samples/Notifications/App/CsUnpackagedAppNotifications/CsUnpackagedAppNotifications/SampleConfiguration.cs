// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace CsUnpackagedAppNotifications
{
    internal static class SampleConfig
    {
        public const string FeatureName = "CsUnpackagedAppNotifications";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = ToastWithAvatar.ScenarioName, ClassName = typeof(Scenario1_ToastWithAvatar).FullName },
            new Scenario() { Title = ToastWithTextBox.ScenarioName, ClassName = typeof(Scenario2_ToastWithTextBox).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
