// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Windows;

namespace CsWpfInstancing
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            if (Program.OutputStack != null && Program.OutputStack.Count > 0)
            {
                foreach (string message in Program.OutputStack)
                {
                    OutputMessage(message);
                }
                Program.OutputStack.Clear();
            }
        }

        public void OutputMessage(string message)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                StatusListView.Items.Add(message);
            }));
        }

        private void ActivationInfoButton_Click(object sender, RoutedEventArgs e)
        {
            Program.GetActivationInfo();
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            OutputMessage("Registering for file activation");
            Program.RegisterForFileActivation();
        }

        private void UnregisterButton_Click(object sender, RoutedEventArgs e)
        {
            OutputMessage("Unregistering for file activation");
            Program.UnregisterForFileActivation();
        }

    }
}
