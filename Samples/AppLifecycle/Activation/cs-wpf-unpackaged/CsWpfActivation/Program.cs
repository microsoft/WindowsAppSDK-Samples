﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsWpfActivation
{
    public class Program
    {
        // Windows App SDK version.
        static uint majorMinorVersion = 0x00010000;
        static string versionTag = "";

        [STAThread]
        static void Main(string[] args)
        {
            // Initialize Windows App SDK for unpackaged apps.            
            int result;
            Bootstrap.TryInitialize(majorMinorVersion, versionTag, out result);
            if (result == 0)
            {
                App app = new()
                {
                    StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
                };
                app.Run();

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
        }
    }
}
