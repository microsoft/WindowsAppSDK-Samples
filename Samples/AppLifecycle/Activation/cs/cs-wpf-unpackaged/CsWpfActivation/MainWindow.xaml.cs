// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.Windows.AppLifecycle;
using System;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using Windows.ApplicationModel.Activation;
using Windows.Storage;

namespace CsWpfActivation
{
    public partial class MainWindow : Window
    {
        private string executablePath;
        private string executablePathAndIconIndex;

        public MainWindow()
        {
            InitializeComponent();

            executablePath = Process.GetCurrentProcess().MainModule.FileName;
            executablePathAndIconIndex = $"{executablePath},0";
        }

        private void OutputMessage(string message)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                StatusListView.Items.Add(message);
            }));
        }

        private void ActivationInfoButton_Click(object sender, RoutedEventArgs e)
        {
            GetActivationInfo();
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            OutputMessage("Registering for rich activation");

            // Register one or more supported filetypes, 
            // an icon (specified by binary file path plus resource index),
            // a display name to use in Shell and Settings,
            // zero or more verbs for the File Explorer context menu,
            // and the path to the EXE to register for activation.
            string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
            string[] verbs = { "view", "edit" };
            ActivationRegistrationManager.RegisterForFileTypeActivation(
                myFileTypes,
                executablePathAndIconIndex,
                "Contoso File Types",
                verbs,
                executablePath
            );

            // Register a URI scheme for protocol activation,
            // specifying the scheme name, icon, display name and EXE path.
            ActivationRegistrationManager.RegisterForProtocolActivation(
                "foo",
                executablePathAndIconIndex,
                "Contoso Foo Protocol",
                executablePath
            );

            // Register for startup activation.
            ActivationRegistrationManager.RegisterForStartupActivation(
                "ContosoStartupId",
                executablePath
            );
        }

        private void UnregisterButton_Click(object sender, RoutedEventArgs e)
        {
            OutputMessage("Unregistering for rich activation");

            // Unregister one or more registered filetypes.
            try
            {
                string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
                ActivationRegistrationManager.UnregisterForFileTypeActivation(
                    myFileTypes,
                    executablePath
                );
            }
            catch (Exception ex)
            {
                OutputMessage($"Error unregistering file types {ex.Message}");
            }

            // Unregister a protocol scheme.
            ActivationRegistrationManager.UnregisterForProtocolActivation(
                "foo",
                "");

            // Unregister for startup activation.
            ActivationRegistrationManager.UnregisterForStartupActivation(
                "ContosoStartupId");
        }

        private void GetActivationInfo()
        {
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            OutputMessage($"ActivationKind: {kind}");

            if (kind == ExtendedActivationKind.Launch)
            {
                if (args.Data is ILaunchActivatedEventArgs launchArgs)
                {
                    string argString = launchArgs.Arguments;
                    string[] argStrings = argString.Split();
                    foreach (string arg in argStrings)
                    {
                        if (!string.IsNullOrWhiteSpace(arg))
                        {
                            OutputMessage(arg);
                        }
                    }
                }
            }
            else if (kind == ExtendedActivationKind.File)
            {
                if (args.Data is IFileActivatedEventArgs fileArgs)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    OutputMessage(file.Name);
                }
            }
            else if (kind == ExtendedActivationKind.Protocol)
            {
                if (args.Data is IProtocolActivatedEventArgs protocolArgs)
                {
                    Uri uri = protocolArgs.Uri;
                    OutputMessage(uri.AbsoluteUri);
                }
            }
            else if (kind == ExtendedActivationKind.StartupTask)
            {
                if (args.Data is IStartupTaskActivatedEventArgs startupArgs)
                {
                    OutputMessage(startupArgs.TaskId);
                }
            }
        }

    }
}
