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

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "Basic Input C# sample";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title="Input Events", ClassType=typeof(Scenario1)},
            new Scenario() { Title="PointerPoint Properties", ClassType=typeof(Scenario2)},
            new Scenario() { Title="Device Capabilities", ClassType=typeof(Scenario3)},
            new Scenario() { Title="XAML Manipulations", ClassType=typeof(Scenario4)},
            new Scenario() { Title="Gesture Recognizer", ClassType=typeof(Scenario5)},
            new Scenario() { Title="Pointer Tracking", ClassType=typeof(Scenario6)}
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
