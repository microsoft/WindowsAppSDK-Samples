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
        public const string FEATURE_NAME = "Camera C# sample";

        // Scenarios are grouped by camera feature. Each group becomes a NavigationView entry:
        // single-scenario groups are leaf items; multi-scenario groups are expandable headers.
        // The code for each feature lives under Scenarios\<Feature>\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Starter kit", new()
            {
                new Scenario { Title = "Camera preview, photo, and video capture", ClassType = typeof(Scenario1_CameraStarterKit) },
            }),
            new FeatureGroup("Manual controls", new()
            {
                new Scenario { Title = "Camera preview and manual controls", ClassType = typeof(Scenario1_CameraManualControls) },
            }),
            new FeatureGroup("Resolution", new()
            {
                new Scenario { Title = "Change camera preview settings", ClassType = typeof(Scenario1_PreviewSettings) },
                new Scenario { Title = "Change preview and photo settings", ClassType = typeof(Scenario2_PhotoSettings) },
                new Scenario { Title = "Match aspect ratios", ClassType = typeof(Scenario3_AspectRatio) },
            }),
            new FeatureGroup("Profile", new()
            {
                new Scenario { Title = "Locate record-specific profile", ClassType = typeof(Scenario1_SetRecordProfile) },
                new Scenario { Title = "Query profile for concurrency", ClassType = typeof(Scenario2_ConcurrentProfile) },
                new Scenario { Title = "Query profile for HDR support", ClassType = typeof(Scenario3_EnableHdrProfile) },
            }),
            new FeatureGroup("Advanced capture", new()
            {
                new Scenario { Title = "Advanced photo capture", ClassType = typeof(Scenario1_CameraAdvancedCapture) },
            }),
            new FeatureGroup("Get preview frame", new()
            {
                new Scenario { Title = "Get a preview frame", ClassType = typeof(Scenario1_GetPreviewFrame) },
            }),
            new FeatureGroup("Face detection", new()
            {
                new Scenario { Title = "Camera preview with face detection", ClassType = typeof(Scenario1_CameraFaceDetection) },
            }),
            new FeatureGroup("Frames", new()
            {
                new Scenario { Title = "Shared mode access to color, depth and infrared frame sources", ClassType = typeof(Scenario1_DisplayDepthColorIR) },
                new Scenario { Title = "Find and display all media frame sources", ClassType = typeof(Scenario2_FindAvailableSourceGroups) },
            }),
            new FeatureGroup("Video stabilization", new()
            {
                new Scenario { Title = "Video stabilization", ClassType = typeof(Scenario1_CameraVideoStabilization) },
            }),
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public class FeatureGroup
    {
        public FeatureGroup(string title, List<Scenario> scenarios)
        {
            Title = title;
            Scenarios = scenarios;
        }

        public string Title { get; set; }
        public List<Scenario> Scenarios { get; set; }
    }
}
