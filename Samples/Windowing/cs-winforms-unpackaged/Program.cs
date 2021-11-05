// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Windows.Forms;
using Microsoft.Windows.ApplicationModel;

namespace winforms_unpackaged_app
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            // Initialize dynamic dependencies so we can consume the Windows App SDK APIs from this unpackaged app.
            // Take a dependency on Windows App SDK v1.0.0-preview1.
            int hr = MddBootstrap.Initialize(0x00010000, "preview3");
            if (hr < 0)
            {
                MessageBox.Show("Fail to initialize Dynamic Dependencies with error 0x" + hr.ToString("X"), "Error!");
                return;
            }

            Application.Run(new AppForm());

            MddBootstrap.Shutdown();
        }
    }
}
