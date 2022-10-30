// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace cs_winui_packaged
{
    internal static class SampleConfig
    {
        public const string FeatureName = "Application Recovery and Restart";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Update reboot registration", ClassName = typeof(S1_UpdateReboot).FullName },
            new Scenario() { Title = "Crash recovery registration", ClassName = typeof(S2_CrashRecovery).FullName },
            new Scenario() { Title = "Reboot type detection", ClassName = typeof(S3_RebootTypeDetection).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
