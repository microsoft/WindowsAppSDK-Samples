// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
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

        private const string PackageName = "MultiHeadedSparseSample";

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

                var packageManager = new PackageManager();

                // Check if already registered
                var existing = packageManager.FindPackagesForUser(string.Empty)
                    .FirstOrDefault(p => p.Id.Name == PackageName);
                if (existing != null)
                {
                    AppendOutput("Package is already registered.");
                    return;
                }

                AppendOutput($"Registering sparse package: {msixPath}");

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
                var packageManager = new PackageManager();
                var package = packageManager.FindPackagesForUser(string.Empty)
                    .FirstOrDefault(p => p.Id.Name == PackageName);

                if (package == null)
                {
                    AppendOutput("No package is currently registered.");
                    return;
                }

                AppendOutput($"Unregistering package: {package.Id.FullName}");
                await packageManager.RemovePackageAsync(package.Id.FullName);
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
            // Check if the sparse package is registered
            var packageManager = new PackageManager();
            var package = packageManager.FindPackagesForUser(string.Empty)
                .FirstOrDefault(p => p.Id.Name == PackageName);

            if (package != null)
            {
                // Launch through the package activation manager so the process gets identity
                string aumid = $"{package.Id.FamilyName}!App";
                AppendOutput($"Activating with AUMID: {aumid}");
                ActivateByAumid(aumid);
            }
            else
            {
                // No package registered, just relaunch the exe directly
                var exePath = Process.GetCurrentProcess().MainModule.FileName;
                Process.Start(exePath);
            }

            Application.Current.Shutdown();
        }

        private void LaunchSecondaryButton_Click(object sender, RoutedEventArgs e)
        {
            var packageManager = new PackageManager();
            var package = packageManager.FindPackagesForUser(string.Empty)
                .FirstOrDefault(p => p.Id.Name == PackageName);

            if (package != null)
            {
                string aumid = $"{package.Id.FamilyName}!SecondaryApp";
                AppendOutput($"Launching secondary app with AUMID: {aumid}");
                ActivateByAumid(aumid);
            }
            else
            {
                AppendOutput("Package not registered. Register the package first.");
            }
        }

        private static void ActivateByAumid(string aumid)
        {
            var clsid = new Guid("45ba127d-10a8-46ea-8ab7-56ea9078943c");
            var iid = new Guid("2e941141-7f97-4756-ba1d-9decde894a3d");

            uint hr = CoCreateInstance(ref clsid, IntPtr.Zero, 0x4 /* CLSCTX_LOCAL_SERVER */, ref iid, out object obj);
            if (hr != 0)
                throw new COMException("Failed to create ApplicationActivationManager", (int)hr);

            var aam = (IApplicationActivationManager)obj;
            aam.ActivateApplication(aumid, string.Empty, 0, out _);
        }

        private void AppendOutput(string message)
        {
            OutputTextBox.Text += message + Environment.NewLine;
            OutputTextBox.ScrollToEnd();
        }

        [DllImport("ole32.dll")]
        private static extern uint CoCreateInstance(
            ref Guid rclsid, IntPtr pUnkOuter, uint dwClsContext,
            ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppv);

        [ComImport, Guid("2e941141-7f97-4756-ba1d-9decde894a3d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IApplicationActivationManager
        {
            IntPtr ActivateApplication([In] string appUserModelId, [In] string arguments, [In] uint options, [Out] out uint processId);
            IntPtr ActivateForFile([In] string appUserModelId, [In] IntPtr itemArray, [In] string verb, [Out] out uint processId);
            IntPtr ActivateForProtocol([In] string appUserModelId, [In] IntPtr itemArray, [Out] out uint processId);
        }
    }
}
