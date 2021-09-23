// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;

namespace CsWpfState
{
    public class Program
    {
        // UNDONE: A packaged app does not need to initialize the Windows App SDK for unpackaged support.
        // Windows App SDK version.
        //static uint majorMinorVersion = 0x00010000;
        //static string versionTag = "preview1";

        [STAThread]
        static void Main(string[] args)
        {
            // UNDONE: Initialize Windows App SDK for unpackaged apps.            
            //int result = MddBootstrap.Initialize(majorMinorVersion, versionTag);
            //if (result == 0)
            //{
                App app = new()
                {
                    StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
                };
                app.Run();

            //    // UNDONE: Uninitialize Windows App SDK.
            //    MddBootstrap.Shutdown();
            //}
        }
    }
}
