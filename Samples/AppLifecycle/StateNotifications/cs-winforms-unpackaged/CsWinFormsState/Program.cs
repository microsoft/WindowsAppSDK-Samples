// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;
using global::Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsWinFormsState
{
    static class Program
    {
        // Windows App SDK version.
        static var majorMinorVersion = global::Microsoft.WindowsAppSDK.Release.MajorMinor;
        static var versionTag = global::Microsoft.WindowsAppSDK.Release.VersionTag;
        static var minVersion = new global::Microsoft.Windows.ApplicationModel.DynamicDependency.PackageVersion(Microsoft.WindowsAppSDK.Runtime.Version.UInt64);

        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Initialize Windows App SDK for unpackaged apps.            
            int result = 0;
            if (!Bootstrap.TryInitialize(majorMinorVersion, versionTag, minVersion, out result))
            {
                Application.Run(new MainForm());

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
        }
    }
}
