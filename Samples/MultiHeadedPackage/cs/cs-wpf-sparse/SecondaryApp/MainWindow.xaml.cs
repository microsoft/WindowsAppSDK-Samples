// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace SecondaryApp
{
    public partial class MainWindow : Window
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern int GetCurrentPackageFullName(ref int packageFullNameLength, StringBuilder packageFullName);

        public MainWindow()
        {
            InitializeComponent();
            CheckIdentityStatus();
        }

        private void CheckIdentityStatus()
        {
            int length = 0;
            int result = GetCurrentPackageFullName(ref length, null);

            if (result == 15700) // APPMODEL_ERROR_NO_PACKAGE
            {
                StatusText.Text = "No package identity (sparse package not registered)";
            }
            else
            {
                var sb = new StringBuilder(length);
                GetCurrentPackageFullName(ref length, sb);
                StatusText.Text = $"Package identity: {sb}";
            }
        }
    }
}
