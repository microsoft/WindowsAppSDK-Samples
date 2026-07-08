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
        public const string FEATURE_NAME = "CustomUsbDeviceAccess C# sample";

        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Custom USB device access", new()
            {
                new Scenario { Title = "Connect/Disconnect", ClassType = typeof(Scenario1_ConnectDisconnect) },
                new Scenario { Title = "Control Transfer", ClassType = typeof(Scenario2_ControlTransfer) },
                new Scenario { Title = "Interrupt Pipes", ClassType = typeof(Scenario3_InterruptPipes) },
                new Scenario { Title = "Bulk Pipes", ClassType = typeof(Scenario4_BulkPipes) },
                new Scenario { Title = "USB Descriptors", ClassType = typeof(Scenario5_UsbDescriptors) },
                new Scenario { Title = "Interface Settings", ClassType = typeof(Scenario6_InterfaceSettings) },
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
