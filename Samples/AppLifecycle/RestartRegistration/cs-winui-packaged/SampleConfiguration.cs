// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace cs_winui_packaged
{
    public static class Kernel32
    {
        public delegate int RecoveryDelegate(IntPtr parameterData);

        [DllImport("kernel32.dll")]
        public static extern int RegisterApplicationRecoveryCallback(RecoveryDelegate recoveryCallback, IntPtr parameter, uint pingInterval, uint flags);

        [DllImport("kernel32.dll")]
        public static extern int ApplicationRecoveryInProgress(out bool canceled);

        [DllImport("kernel32.dll")]
        public static extern void ApplicationRecoveryFinished(bool success);

        [DllImport("kernel32.dll")]
        public static extern int UnregisterApplicationRecoveryCallback();

        [DllImport("kernel32.dll")]
        public static extern int RegisterApplicationRestart([MarshalAs(UnmanagedType.LPWStr)] string commandLineArgs, RestartRestrictions flags);

        [DllImport("kernel32.dll")]
        public static extern int UnregisterApplicationRestart();

        [DllImport("kernel32.dll")]
        public static extern int GetApplicationRestartSettings(SafeHandle process, IntPtr commandLine, ref uint size, out RestartRestrictions flags);


        [Flags]
        public enum RestartRestrictions
        {
            None = 0,
            NotOnCrash = 1,
            NotOnHang = 2,
            NotOnPatch = 4,
            NotOnReboot = 8,
        }
    }

    internal static class SampleConfig
    {
        public const string FeatureName = "Application Recovery and Restart";
        public static ElementTheme CurrentTheme = ElementTheme.Default;
    }

    public partial class MainPage : Page
    {
        private readonly List<Scenario> scenarios = new()
        {
            new Scenario() { Title = "Update reboot registration", ClassName = typeof(S1_UpdateReboot).FullName },
            new Scenario() { Title = "Crash recovery registration", ClassName = typeof(S2_CrashRecovery).FullName },
            new Scenario() { Title = "Reboot type detection", ClassName = typeof(S3_RebootTypeDetection).FullName }
        };
    }

    public class Scenario
    {
        public string Title { get; set; }
        public string ClassName { get; set; }
    }
}
