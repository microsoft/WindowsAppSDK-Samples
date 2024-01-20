// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace CsWinFormsInstancing
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        public MainForm(List<string> info)
        {
            InitializeComponent();

            // If we have a backlog of messages, show them now.
            if (info != null && info.Count > 0)
            {
                foreach (string message in info)
                {
                    OutputMessage(message);
                }
                info.Clear();
            }
        }

        public void OutputMessage(string message)
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

        private void ActivationInfoButton_Click(object sender, EventArgs e)
        {
            Program.GetActivationInfo();
        }

        private void RegisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Registering for file activation");
            Program.RegisterForFileActivation();
        }

        private void UnregisterButton_Click(object sender, EventArgs e)
        {
            OutputMessage("Unregistering for file activation");
            Program.UnregisterForFileActivation();
        }

    }
}
