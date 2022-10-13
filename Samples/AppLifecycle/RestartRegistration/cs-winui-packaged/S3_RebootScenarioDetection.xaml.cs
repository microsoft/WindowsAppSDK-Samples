// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using static cs_winui_packaged.Kernel32;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace cs_winui_packaged
{
    public enum RestartType
    {
        Normal = 0,
        Abnormal = 1,
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S3_RebootScenarioDetection : Page
    {
        public S3_RebootScenarioDetection()
        {
            this.InitializeComponent();
            setRestartTypeText();

            Kernel32.RegisterApplicationRestart(RestartType.Normal.ToString(), Kernel32.RestartRestrictions.None);

            // Register recovery callback to update restart argument in case of abnormal restart
            Kernel32.RegisterApplicationRecoveryCallback(new RecoveryDelegate(p =>
            {
                Kernel32.ApplicationRecoveryInProgress(out bool canceled);
                if (canceled)
                {
                    Environment.Exit(2);
                }

                Kernel32.RegisterApplicationRestart(RestartType.Abnormal.ToString(), Kernel32.RestartRestrictions.NotOnPatch | Kernel32.RestartRestrictions.NotOnReboot);

                ApplicationRecoveryFinished(true);
                return 0;
            }), IntPtr.Zero, 5000, 0);
        }

        private void setRestartTypeText()
        {
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 1)
            {
                commandLineArguments = commandLineArguments.Skip(1).ToArray();
                restartTypeTextBlock.Text = "App recovered from " + commandLineArguments[0].ToLower() + " restart.";
            }
        }

        private void crash_Click(object sender, RoutedEventArgs e)
        {
            for (int i = 60; i >= 0; i--)
            {
                System.Threading.Thread.Sleep(1000);
            }

            throw new Exception("Fatal crash!");
        }

        private void restart_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Windows.AppLifecycle.AppInstance.Restart(RestartType.Normal.ToString());
        }
    }
}
