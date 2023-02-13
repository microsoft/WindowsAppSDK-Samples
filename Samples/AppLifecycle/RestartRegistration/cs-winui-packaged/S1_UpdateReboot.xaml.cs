// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Linq;
using System.Reflection.Emit;
using Windows.Win32.System.Recovery;
using static Windows.Win32.PInvoke;

namespace cs_winui_packaged
{
    public partial class S1_UpdateReboot : Page
    {
        public S1_UpdateReboot()
        {
            this.InitializeComponent();
            setRecoveredMassage();
        }

        private void setRecoveredMassage()
        {
            // Using Environment.GetCommandLineArgs() to retrieve the command line arguments
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 1)
            {
                commandLineArguments = commandLineArguments.Skip(1).ToArray();
                recoveredMessage.Text = String.Join(" ", commandLineArguments);
                recoveryArea.Visibility = Visibility.Visible;
            }
        }

        private void message_TextChanged(object sender, TextChangedEventArgs e)
        {
            unsafe
            {
                // Update registration with new arguments (only if already registered)
                uint flags;
                uint size = 0;
                if (GetApplicationRestartSettings(System.Diagnostics.Process.GetCurrentProcess().SafeHandle, null, ref size, &flags) == 0)
                {
                    String restartArgsInput = messageInput.Text;
                    if (RegisterApplicationRestart(restartArgsInput, (REGISTER_APPLICATION_RESTART_FLAGS)flags) == 0)
                    {
                        MainPage.Current.NotifyUser("Updated registration", InfoBarSeverity.Informational);
                    }
                }
            }
        }

        private void register_Click(object sender, RoutedEventArgs e)
        {
            // Note that even after successful registration, the application will only recover if running for more than 60 seconds.
            if (RegisterApplicationRestart(messageInput.Text, REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_CRASH | REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_HANG) == 0)
            {
                unregisterButton.IsEnabled = true;
                MainPage.Current.NotifyUser("Registered for restart", InfoBarSeverity.Success);
            }
            else
            {
                MainPage.Current.NotifyUser("Failed to register for restart", InfoBarSeverity.Error);
            }
        }

        private void unregister_Click(object sender, RoutedEventArgs e)
        {
            if (UnregisterApplicationRestart() == 0)
            {
                unregisterButton.IsEnabled = false;
                MainPage.Current.NotifyUser("Unregistered from restart", InfoBarSeverity.Success);
            }
            else
            {
                MainPage.Current.NotifyUser("Failed to unregister from restart", InfoBarSeverity.Error);
            }
        }
    }
}
