// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;

namespace CsWpfState
{
    public class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            App app = new()
            {
                StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
            };
            app.Run();
        }
    }
}
