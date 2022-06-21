// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace Input
{
    internal static class SampleConfig
    {
        public const string FeatureName = "Input";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Gesture Recognizer", ClassName = typeof(GestureRecognizer).FullName },
            new Scenario() { Title = "Gesture Recognizer Manipulations", ClassName = typeof(GestureRecognizerManipulation).FullName },
            new Scenario() { Title = "Cursor", ClassName = typeof(InputCursor).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
