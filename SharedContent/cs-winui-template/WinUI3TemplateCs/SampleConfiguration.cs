//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Microsoft.UI.Xaml.Controls;

namespace WinUI3TemplateCs
{
    public static class Settings
    {
        public const string FeatureName = "Windows App SDK Template C# Sample";
        public static string CurrentTheme;
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
