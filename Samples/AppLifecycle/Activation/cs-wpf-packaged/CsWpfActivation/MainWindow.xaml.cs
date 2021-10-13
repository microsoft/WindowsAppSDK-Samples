// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// NOTES. This app is cloned from the unpackaged version. The key differences are as follows:
// 1. A packaged app cannot use the Register/Unregister APIs for rich activation; instead it declares activation kinds in its manifest.
// 2. A packaged app does not need to initialize the Windows App SDK for unpackaged support.
// 3. The Package project must include a reference to the Windows App SDK NuGet in addition to the app project itself.
// 4. A packaged app can declare rich activation extensions in the manifest, and retrieve activation arguments via the Windows App SDK GetActivatedEventArgs API.
// 5. A packaged app can use the UWP GetActivatedEventArgs API instead of the Windows App SDK version.
// 6. MddBootstrap.cs is not needed.

using WindowsAppSdk = Microsoft.Windows.AppLifecycle;
using System;
using System.Linq;
using System.Windows;
using Windows.ApplicationModel.Activation;
using Windows.Storage;

namespace CsWpfActivation
{
    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        private void OutputMessage(string message)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                StatusListView.Items.Add(message);
            }));
        }

        private void ActivationInfoWasButton_Click(object sender, RoutedEventArgs e)
        {
            GetActivationInfoWas();
        }

        private void ActivationInfoUwpButton_Click(object sender, RoutedEventArgs e)
        {
            GetActivationInfoUwp();
        }

        private void GetActivationInfoWas()
        {
            // The Windows App SDK GetActivatedEventArgs returns an AppActivationArguments
            // object, which exposes the specific activation type via its Data property.
            WindowsAppSdk.AppActivationArguments args = 
                WindowsAppSdk.AppInstance.GetCurrent().GetActivatedEventArgs();

            // The Windows App SDK includes the ExtendedActivationKind enum, which
            // includes and extends the UWP ActivationKind enum.
            WindowsAppSdk.ExtendedActivationKind kind = args.Kind;
            OutputMessage($"ActivationKind: {kind}");

            if (kind == WindowsAppSdk.ExtendedActivationKind.Launch)
            {
                if (args.Data is ILaunchActivatedEventArgs launchArgs)
                {
                    string argString = launchArgs.Arguments;
                    string[] argStrings = argString.Split();
                    foreach (string arg in argStrings)
                    {
                        if (!string.IsNullOrWhiteSpace(arg))
                        {
                            OutputMessage(arg);
                        }
                    }
                }
            }
            else if (kind == WindowsAppSdk.ExtendedActivationKind.File)
            {
                if (args.Data is IFileActivatedEventArgs fileArgs)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    OutputMessage(file.Name);
                }
            }
            else if (kind == WindowsAppSdk.ExtendedActivationKind.Protocol)
            {
                if (args.Data is IProtocolActivatedEventArgs protocolArgs)
                {
                    Uri uri = protocolArgs.Uri;
                    OutputMessage(uri.AbsoluteUri);
                }
            }
            else if (kind == WindowsAppSdk.ExtendedActivationKind.StartupTask)
            {
                if (args.Data is IStartupTaskActivatedEventArgs startupArgs)
                {
                    OutputMessage(startupArgs.TaskId);
                }
            }
        }

        // Use the UWP version of AppInstance::GetActivatedEventArgs.
        private void GetActivationInfoUwp()
        {
            // The UWP GetActivatedEventArgs returns an IActivatedEventArgs,
            // which can be directly cast to the specific activation type it represents.
            IActivatedEventArgs args = 
                Windows.ApplicationModel.AppInstance.GetActivatedEventArgs();
            ActivationKind kind = args.Kind;
            OutputMessage($"ActivationKind: {kind}");

            if (kind == ActivationKind.Launch)
            {
                if (args is ILaunchActivatedEventArgs launchArgs)
                {
                    string argString = launchArgs.Arguments;
                    string[] argStrings = argString.Split();
                    foreach (string arg in argStrings)
                    {
                        if (!string.IsNullOrWhiteSpace(arg))
                        {
                            OutputMessage(arg);
                        }
                    }
                }
            }
            else if (kind == ActivationKind.File)
            {
                if (args is IFileActivatedEventArgs fileArgs)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    OutputMessage(file.Name);
                }
            }
            else if (kind == ActivationKind.Protocol)
            {
                if (args is IProtocolActivatedEventArgs protocolArgs)
                {
                    Uri uri = protocolArgs.Uri;
                    OutputMessage(uri.AbsoluteUri);
                }
            }
            else if (kind == ActivationKind.StartupTask)
            {
                if (args is IStartupTaskActivatedEventArgs startupArgs)
                {
                    OutputMessage(startupArgs.TaskId);
                }
            }
        }

    }
}
