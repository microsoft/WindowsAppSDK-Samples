// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows.Forms;

namespace CsWinFormsEnv
{
    static class Program
    {

        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Application.Run(new MainForm());

        }
    }
}
