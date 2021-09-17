﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;

namespace CsWpfActivation
{
    public class Program
    {
        static uint majorMinorVersion = 0x00010000;
        static string versionTag = "preview1";

        [STAThread]
        static void Main(string[] args)
        {
            // Initialize WASDK for unpackaged apps.            
            int result = MddBootstrap.Initialize(majorMinorVersion, versionTag);
            if (result == 0)
            {
                App app = new()
                {
                    StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
                };
                app.Run();

                // Uninitialize WASDK.
                MddBootstrap.Shutdown();
            }
        }
    }
}
