// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Linq;
using System.Reflection.Emit;
using static cs_winui_packaged.Kernel32;

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
            // Update registration with new arguments (only if already registered)
            RestartRestrictions flags;
            uint size = 0;
            if (Kernel32.GetApplicationRestartSettings(System.Diagnostics.Process.GetCurrentProcess().SafeHandle, IntPtr.Zero, ref size, out flags) == 0)
            {
                String restartArgsInput = messageInput.Text;
                if (Kernel32.RegisterApplicationRestart(restartArgsInput, flags) == 0)
                {
                    MainPage.Current.NotifyUser("Updated registration", InfoBarSeverity.Informational);
                }
            }
        }

        private void register_Click(object sender, RoutedEventArgs e)
        {
            // Note that even after successful registration, the application will only recover if running for more than 60 seconds
            String restartArgsInput = messageInput.Text;
            int exitCode = Kernel32.RegisterApplicationRestart(restartArgsInput, Kernel32.RestartRestrictions.NotOnCrash | Kernel32.RestartRestrictions.NotOnHang);

            if (exitCode == 0)
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
            int exitCode = Kernel32.UnregisterApplicationRestart();

            if (exitCode == 0)
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
