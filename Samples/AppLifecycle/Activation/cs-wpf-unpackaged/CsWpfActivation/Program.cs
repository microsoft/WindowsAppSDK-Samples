// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using global::Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsWpfActivation
{
    public class Program
    {
        // Windows App SDK version.
        static var majorMinorVersion = global::Microsoft.WindowsAppSDK.Release.MajorMinor;
        static var versionTag = global::Microsoft.WindowsAppSDK.Release.VersionTag;
        static var minVersion = new global::Microsoft.Windows.ApplicationModel.DynamicDependency.PackageVersion(Microsoft.WindowsAppSDK.Runtime.Version.UInt64);

        [STAThread]
        static void Main(string[] args)
        {
            // Initialize Windows App SDK for unpackaged apps.            
            int result = 0;
            if (!Bootstrap.TryInitialize(majorMinorVersion, versionTag, minVersion, out result))
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
