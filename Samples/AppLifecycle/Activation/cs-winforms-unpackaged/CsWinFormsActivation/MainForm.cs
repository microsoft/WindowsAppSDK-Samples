// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Linq;
using System.Windows.Forms;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using Microsoft.Windows.AppLifecycle;

namespace CsWinFormsActivation
{
    public partial class MainForm : Form
    {

        #region Init

        private string executablePath;
        private string executablePathAndIconIndex;

        public MainForm()
        {
            InitializeComponent();

            executablePath = Application.ExecutablePath;
            executablePathAndIconIndex = $"{executablePath},0";
        }

        private void OutputMessage(string message)
        {
            if (!listView1.InvokeRequired)
            {
                listView1.Items.Add(message);
            }
            else
            {
                listView1.Invoke((Action)(delegate { listView1.Items.Add(message); }));
            }
        }

        #endregion


        #region Button Click handlers

        private void ActivationInfoButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Getting activation info");
            GetActivationInfo();
        }

        private void RegisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Registering for rich activation");
            RegisterForRichActivation();
        }

        private void UnregisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Unregistering for rich activation");
            UnregisterForRichActivation();
        }

        #endregion


        #region Rich activation

        private void GetActivationInfo()
        {
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            OutputMessage($"ActivationKind: {kind}");

            if (kind == ExtendedActivationKind.Launch)
            {
                ILaunchActivatedEventArgs launchArgs = args.Data as ILaunchActivatedEventArgs;
                if (launchArgs != null)
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
                IFileActivatedEventArgs fileArgs = args.Data as IFileActivatedEventArgs;
                if (fileArgs != null)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    OutputMessage(file.Name);
                }
            }
            else if (kind == ExtendedActivationKind.Protocol)
            {
                IProtocolActivatedEventArgs protocolArgs = args.Data as IProtocolActivatedEventArgs;
                if (protocolArgs != null)
                {
                    Uri uri = protocolArgs.Uri;
                    OutputMessage(uri.AbsoluteUri);
                }
            }
            else if (kind == ExtendedActivationKind.StartupTask)
            {
                IStartupTaskActivatedEventArgs startupArgs = args.Data as IStartupTaskActivatedEventArgs;
                if (startupArgs != null)
                {
                    OutputMessage(startupArgs.TaskId);
                }
            }
        }

        private void RegisterForRichActivation()
        {
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

        private void UnregisterForRichActivation()
        {
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

        #endregion

    }
}
