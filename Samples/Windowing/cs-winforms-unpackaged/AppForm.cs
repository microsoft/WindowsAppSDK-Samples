// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Windows.Forms;
using Microsoft.UI;
using Microsoft.UI.Windowing;

namespace winforms_unpackaged_app
{
    public partial class AppForm : Form
    {
        public String m_windowTitle = "WinForms Desktop C# Sample App";
        AppWindow m_mainAppWindow;
        public AppForm()
        {
            this.InitializeComponent();
            this.Text = m_windowTitle;
            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
           m_mainAppWindow = AppWindowExtensions.GetAppWindowFromWinformsWindow(this);
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            windowBasics1.BringToFront();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            presenters1.BringToFront();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            titleBar1.BringToFront();
        }
    }
}
