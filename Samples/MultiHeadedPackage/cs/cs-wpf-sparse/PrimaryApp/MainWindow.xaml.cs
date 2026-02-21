// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using Windows.Management.Deployment;

namespace PrimaryApp
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
                AppendOutput("App is running without package identity.");
                AppendOutput("Click 'Register Package' to register the sparse package.");
            }
            else
            {
                var sb = new StringBuilder(length);
                GetCurrentPackageFullName(ref length, sb);
                StatusText.Text = $"Package identity: {sb}";
                AppendOutput($"App is running with package identity: {sb}");
            }
        }

        private async void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var exePath = Process.GetCurrentProcess().MainModule.FileName;
                var exeDir = Path.GetDirectoryName(exePath);
                var msixPath = Path.Combine(exeDir, "PrimaryApp.msix");

                if (!File.Exists(msixPath))
                {
                    AppendOutput($"Error: Could not find sparse package at: {msixPath}");
                    AppendOutput("Build the project first to generate the sparse MSIX package.");
                    return;
                }

                AppendOutput($"Registering sparse package: {msixPath}");

                var packageManager = new PackageManager();
                var packageUri = new Uri(msixPath);
                var options = new AddPackageOptions
                {
                    AllowUnsigned = true,
                    ExternalLocationUri = new Uri(exeDir)
                };

                await packageManager.AddPackageByUriAsync(packageUri, options);
                AppendOutput("Sparse package registered successfully!");
                AppendOutput("Click 'Restart' to relaunch with package identity.");
            }
            catch (Exception ex)
            {
                AppendOutput($"Registration failed: {ex.Message}");
            }
        }

        private async void UnregisterButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                int length = 0;
                GetCurrentPackageFullName(ref length, null);
                var sb = new StringBuilder(length);
                int result = GetCurrentPackageFullName(ref length, sb);

                if (result == 15700) // APPMODEL_ERROR_NO_PACKAGE
                {
                    AppendOutput("No package is currently registered.");
                    return;
                }

                string packageFullName = sb.ToString();
                AppendOutput($"Unregistering package: {packageFullName}");

                var packageManager = new PackageManager();
                await packageManager.RemovePackageAsync(packageFullName);

                AppendOutput("Package unregistered successfully!");
                AppendOutput("Click 'Restart' to relaunch without package identity.");
            }
            catch (Exception ex)
            {
                AppendOutput($"Unregistration failed: {ex.Message}");
            }
        }

        private void RestartButton_Click(object sender, RoutedEventArgs e)
        {
            var exePath = Process.GetCurrentProcess().MainModule.FileName;
            Process.Start(exePath);
            Application.Current.Shutdown();
        }

        private void AppendOutput(string message)
        {
            OutputTextBox.Text += message + Environment.NewLine;
            OutputTextBox.ScrollToEnd();
        }
    }
}
