// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;

namespace Windowing
{
    internal static class Settings
    {
        public const string FeatureName = "Windowing WinUI C# Sample";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Window Basics", ClassName = typeof(WindowBasics).FullName },
            new Scenario() { Title = "Title Bar", ClassName = typeof(TitleBar).FullName },
            new Scenario() { Title = "Presenters", ClassName = typeof(Presenters).FullName },
            new Scenario() { Title = "Z-Order", ClassName = typeof(ZOrder).FullName}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
