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

        private void AddVarsButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Add Environment Variables");
            AddEnvironmentVariables();
        }

        private void DelVarsButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Delete Environment Variables");
            DeleteEnvironmentVariables();
        }

        private void AddPathButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Add to PATH");
            AddToPath();
        }

        private void DelPathButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Remove from PATH");
            RemoveFromPath();
        }

        private void AddPathExtButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Add to PATHEXT");
            AddToPathExt();
        }

        private void DelPathExtButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Remove from PATHEXT");
            RemoveFromPathExt();
        }

        #endregion


        #region Manipulate environment variables

        private void GetEnvironmentVariables()
        {
            // The EnvironmentManager APIs are only supported from OS builds 19044 onwards.
            if (EnvironmentManager.IsSupported)
            {
                // Get environment variables for the current user.
                var userManager = EnvironmentManager.GetForUser();
                var tempValue = userManager.GetEnvironmentVariable("TEMP");
                OutputMessage($"TEMP={tempValue}");

                // Get system-wide environment variables.
                var systemManager = EnvironmentManager.GetForMachine();
                var comSpecValue = systemManager.GetEnvironmentVariable("ComSpec");
                OutputMessage($"ComSpec={comSpecValue}");

                // Get all environment variables at a particular level.
                var processManager = EnvironmentManager.GetForProcess();
                var envVars = processManager.GetEnvironmentVariables();
                foreach (var envVar in envVars)
                {
                    OutputMessage($"{envVar.Key}={envVar.Value}");
                }
            }
        }

        private void AddEnvironmentVariables()
        {
            if (EnvironmentManager.IsSupported)
            {
                try
                {
                    // Environment variables set at the process level will persist
                    // only until the process terminates.
                    var processManager = EnvironmentManager.GetForProcess();
                    processManager.SetEnvironmentVariable("MAXITEMS", "1024");
                    var maxItemsValue = processManager.GetEnvironmentVariable("MAXITEMS");
                    OutputMessage($"MAXITEMS={maxItemsValue}");

                    // For packaged apps, environment variables set at the user 
                    // or machine level will persist until the app is uninstalled.
                    // For unpackaged apps, they are not removed on app uninstall.
                    var userManager = EnvironmentManager.GetForUser();
                    userManager.SetEnvironmentVariable("HOMEDIR", "D:\\Foo");
                    var homeDirValue = userManager.GetEnvironmentVariable("HOMEDIR");
                    OutputMessage($"HOMEDIR={homeDirValue}");

                    // To set environment variables at machine level, the app must
                    // be running elevated, otherwise this will result in Access Denied.
                    var systemManager = EnvironmentManager.GetForMachine();
                    systemManager.SetEnvironmentVariable("Fruit", "Banana");
                    var fruitValue = systemManager.GetEnvironmentVariable("Fruit");
                    OutputMessage($"Fruit={fruitValue}");
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        private void DeleteEnvironmentVariables()
        {
            if (EnvironmentManager.IsSupported)
            {
                try
                {
                    // Environment variables set at the process level will persist
                    // only until the process terminates.
                    var processManager = EnvironmentManager.GetForProcess();
                    processManager.SetEnvironmentVariable("MAXITEMS", "");
                    var maxItemsValue = processManager.GetEnvironmentVariable("MAXITEMS");
                    if (maxItemsValue == "")
			        {
                        OutputMessage("MAXITEMS not found");
                    }
                    else
                    {
                        OutputMessage($"MAXITEMS={maxItemsValue}");
                    }

                    // For packaged apps, environment variables set at the user 
                    // or machine level will persist until the app is uninstalled.
                    // For unpackaged apps, they are not removed on app uninstall.
                    var userManager = EnvironmentManager.GetForUser();
                    userManager.SetEnvironmentVariable("HOMEDIR", "");
                    var homeDirValue = userManager.GetEnvironmentVariable("HOMEDIR");
                    if (homeDirValue == "")
                    {
                        OutputMessage("HOMEDIR not found");
                    }
                    else
                    {
                        OutputMessage($"HOMEDIR={homeDirValue}");
                    }

                    // To set environment variables at machine level, the app must
                    // be running elevated, otherwise this will result in Access Denied.
                    var systemManager = EnvironmentManager.GetForMachine();
                    systemManager.SetEnvironmentVariable("Fruit", "");
                    var fruitValue = systemManager.GetEnvironmentVariable("Fruit");
                    if (fruitValue == "")
                    {
                        OutputMessage("Fruit not found");
                    }
                    else
                    {
                        OutputMessage($"Fruit={fruitValue}");
                    }
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        private void AddToPath()
        {
            // You can append paths to the end of the PATH variable, at
            // process, user or machine scope.
            if (EnvironmentManager.IsSupported)
            {
                try
                {
                    var userManager = EnvironmentManager.GetForUser();
                    userManager.AppendToPath("%USERPROFILE%\\ContosoBin");
                    var pathValue = userManager.GetEnvironmentVariable("PATH");
                    OutputMessage($"PATH={pathValue}");
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        private void RemoveFromPath()
        {
            // You can remove paths from the PATH variable that you
            // previously appended.
            // Removal is done with case-insensitive ordinal string comparison,
            // without expanding the %USERPROFILE% environment variable.
            if (EnvironmentManager.IsSupported)
            {
                try
                {
                    var userManager = EnvironmentManager.GetForUser();
                    userManager.RemoveFromPath("%USERPROFILE%\\ContosoBin");
                    var pathValue = userManager.GetEnvironmentVariable("PATH");
                    OutputMessage($"PATH={pathValue}");
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        private void AddToPathExt()
        {
            // You can append extensions to the PATHEXT variable.
            if (EnvironmentManager.IsSupported)
            {
                try
                {
                    var userManager = EnvironmentManager.GetForUser();
                    userManager.AddExecutableFileExtension(".XYZ");
                    var pathExtValue = userManager.GetEnvironmentVariable("PATHEXT");
                    OutputMessage($"PATHEXT={pathExtValue}");
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        private void RemoveFromPathExt()
        {
            // You can remove any values from the PATHEXT variable
            // that you previously added.
            if (EnvironmentManager.IsSupported)
            {
                try
                {var userManager = EnvironmentManager.GetForUser();
                    userManager.AddExecutableFileExtension(".XYZ");
                    var pathExtValue = userManager.GetEnvironmentVariable("PATHEXT");
                    OutputMessage($"PATHEXT={pathExtValue}");
                }
                catch (Exception ex)
                {
                    OutputMessage(ex.Message);
                }
            }
        }

        #endregion

    }
}
