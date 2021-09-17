// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;
using Microsoft.Windows.System;

namespace CsWinFormsEnv
{
    public partial class MainForm : Form
    {

        #region Init

        public MainForm()
        {
            InitializeComponent();
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


        #region Button click handlers

        private void GetVarsButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Get Environment Variables");
            GetEnvironmentVariables();
        }

        private void SetVarsButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Set Environment Variables");
            SetEnvironmentVariables();
        }

        private void PathButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Update PATH");
            UpdatePath();
        }

        private void PathExtButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Update PATHEXT");
            UpdatePathExt();
        }

        #endregion


        #region Register/Unregister callbacks

        private void GetEnvironmentVariables()
        {
            if (EnvironmentManager.IsSupported)
            {
                // Get environment variables for the current user.
                var userManager = EnvironmentManager.GetForUser();
                var TEMP = userManager.GetEnvironmentVariable("TEMP");
                OutputMessage($"TEMP={TEMP}");

                // Get system-wide environment variables.
                var systemManager = EnvironmentManager.GetForMachine();
                var ComSpec = systemManager.GetEnvironmentVariable("ComSpec");
                OutputMessage($"ComSpec={ComSpec}");

                // Get all environment variables at a particular level.
                var processManager = EnvironmentManager.GetForProcess();
                var envVars = processManager.GetEnvironmentVariables();
                foreach (var envVar in envVars)
                {
                    OutputMessage($"{envVar.Key}={envVar.Value}");
                }
            }
        }

        private void SetEnvironmentVariables()
        {
            if (EnvironmentManager.IsSupported)
            {
                // Environment variables set at the process level will persist
                // only until the process terminates.
                var processManager = EnvironmentManager.GetForProcess();
                processManager.SetEnvironmentVariable("MAXITEMS", "1024");
                var MAXITEMS = processManager.GetEnvironmentVariable("MAXITEMS");
                OutputMessage($"MAXITEMS={MAXITEMS}");

                // For packaged apps, environment variables set at the user 
                // or machine level will persist until the app is uninstalled.
                // For unpackaged apps, they are not removed on app uninstall.
                var userManager = EnvironmentManager.GetForUser();
                userManager.SetEnvironmentVariable("HOMEDIR", "D:\\Foo");
                var HOMEDIR = userManager.GetEnvironmentVariable("HOMEDIR");
                OutputMessage($"HOMEDIR={HOMEDIR}");

                var systemManager = EnvironmentManager.GetForMachine();

                // BUG:
                // Unhandled exception at 0x75EBE7D2 in CppWinRtConsoleEnv.exe: 
                // Microsoft C++ exception: winrt::hresult_access_denied at memory location 0x0079F1C0.
                systemManager.SetEnvironmentVariable("Fruit", "Banana");

                var Fruit = systemManager.GetEnvironmentVariable("Fruit");
                OutputMessage($"Fruit={Fruit}");
            }
        }

        private void UpdatePath()
        {
            if (EnvironmentManager.IsSupported)
            {
                // You can append paths to the end of the PATH variable, at
                // process, user or machine scope.
                var userManager = EnvironmentManager.GetForUser();
                userManager.AppendToPath("%USERPROFILE%\\ContosoBin");
                var PATH = userManager.GetEnvironmentVariable("PATH");
                OutputMessage($"PATH={PATH}");

                // You can remove paths from the PATH variable that you
                // previously appended.
                // Removal is done with case-insensitive ordinal string comparison,
                // without expanding the %USERPROFILE% environment variable.
                userManager.RemoveFromPath("%USERPROFILE%\\ContosoBin");
                PATH = userManager.GetEnvironmentVariable("PATH");
                OutputMessage($"PATH={PATH}");
            }
        }

        // You can append extensions to the PATHEXT variable, and
        // remove any you added previously.
        private void UpdatePathExt()
        {
            if (EnvironmentManager.IsSupported)
            {
                var userManager = EnvironmentManager.GetForUser();

                // BUG
                // Exception thrown at 0x75EBE7D2 in CppWinRtConsoleEnv.exe: 
                // Microsoft C++ exception: winrt::hresult_error at memory location 0x00B9F544.
                userManager.AddExecutableFileExtension(".XYZ");
                var PATHEXT = userManager.GetEnvironmentVariable("PATHEXT");
                OutputMessage($"PATHEXT={PATHEXT}");

                userManager.RemoveExecutableFileExtension(".XYZ");
                PATHEXT = userManager.GetEnvironmentVariable("PATHEXT");
                OutputMessage($"PATHEXT={PATHEXT}");
            }
        }

        #endregion

    }
}
