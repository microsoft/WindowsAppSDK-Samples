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
using Windows.Win32.Foundation;
using Windows.Win32.System.Recovery;
using Windows.Win32.System.WindowsProgramming;
using static Windows.Win32.PInvoke;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace cs_winui_packaged
{
    enum RebootType
    {
        Normal,
        Abnormal
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class S3_RebootTypeDetection : Page
    {
        public S3_RebootTypeDetection()
        {
            this.InitializeComponent();
            setRestartTypeText();

            RegisterApplicationRestart(RebootType.Normal.ToString().ToLower(), 0);

            unsafe
            {
                // Register recovery callback to update restart argument in case of abnormal restart
                RegisterApplicationRecoveryCallback(new APPLICATION_RECOVERY_CALLBACK(p =>
                {
                    ApplicationRecoveryInProgress(out BOOL canceled);
                    if (canceled)
                    {
                        Environment.Exit(2);
                    }

                    RegisterApplicationRestart(RebootType.Abnormal.ToString().ToLower(), REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_PATCH | REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_REBOOT);

                    ApplicationRecoveryFinished(true);
                    return 0;
                }), null, 5000, 0);
            }
        }

        private void setRestartTypeText()
        {
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 1)
            {
                commandLineArguments = commandLineArguments.Skip(1).ToArray();
                if (commandLineArguments[0] == RebootType.Normal.ToString().ToLower() || 
                    commandLineArguments[0] == RebootType.Abnormal.ToString().ToLower())
                {
                    restartTypeTextBlock.Text = "App recovered from " + commandLineArguments[0] + " restart.";
                }
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
            Microsoft.Windows.AppLifecycle.AppInstance.Restart(RebootType.Normal.ToString().ToLower());
        }
    }
}
