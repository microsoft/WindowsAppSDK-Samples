// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;

namespace CsWinFormsEnv
{
    static class Program
    {
        static uint majorMinorVersion = 0x00010000;

        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Initialize WASDK for unpackaged apps.            
            int result = MddBootstrap.Initialize(majorMinorVersion);
            if (result == 0)
            {
                Application.Run(new MainForm());

                // Uninitialize WASDK.
                MddBootstrap.Shutdown();
            }
        }
    }
}
