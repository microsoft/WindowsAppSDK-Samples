// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;
using Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsWinFormsState
{
    static class Program
    {
        // Windows App SDK version.
        static uint majorMinorVersion = 0x00010000;
        static string versionTag = "";

        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Initialize Windows App SDK for unpackaged apps.            
            int result;
            Bootstrap.TryInitialize(majorMinorVersion, versionTag, out result);
            if (result == 0)
            {
                Application.Run(new MainForm());

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
        }
    }
}
