// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// NOTES. This app is cloned from the unpackaged version. Redundant code is left in place as comments, so that you can more easily see the differences.
// 1. A packaged app cannot use the Register/Unregister APIs for rich activation.
// 2. A packaged app does not need to initialize the Windows App SDK for unpackaged support.
// 3. The Package project must include a reference to the Windows App SDK NuGet in addition to the app project itself.
// 4. A packaged app can declare rich activation extensions in the manifest, and retrieve activation arguments via the Windows App SDK GetActivatedEventArgs API.
// 5. The app can use the UWP GetActivatedEventArgs API instead.
// 6. MddBootstrap.cs is not needed.

// NOTE: To avoid name conflicts, we'll alias the Windows App SDK namespace.
//using Microsoft.Windows.AppLifecycle;
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
        // UNDONE: A packaged app can't use the Register/Unregister APIs for rich activation.
        //private string executablePath;
        //private string executablePathAndIconIndex;

        public MainWindow()
        {
            InitializeComponent();

            //executablePath = Process.GetCurrentProcess().MainModule.FileName;
            //executablePathAndIconIndex = $"{executablePath},1";
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

        //private void RegisterButton_Click(object sender, RoutedEventArgs e)
        //{
        //    OutputMessage("Registering for rich activation");

        //    // Register one or more supported filetypes, 
        //    // an icon (specified by binary file path plus resource index),
        //    // a display name to use in Shell and Settings,
        //    // zero or more verbs for the File Explorer context menu,
        //    // and the path to the EXE to register for activation.
        //    string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
        //    string[] verbs = { "view", "edit" };
        //    ActivationRegistrationManager.RegisterForFileTypeActivation(
        //        myFileTypes,
        //        executablePathAndIconIndex,
        //        "Contoso File Types",
        //        verbs,
        //        executablePath
        //    );

        //    // Register a URI scheme for protocol activation,
        //    // specifying the scheme name, icon, display name and EXE path.
        //    ActivationRegistrationManager.RegisterForProtocolActivation(
        //        "foo",
        //        executablePathAndIconIndex,
        //        "Contoso Foo Protocol",
        //        executablePath
        //    );

        //    // Register for startup activation.
        //    ActivationRegistrationManager.RegisterForStartupActivation(
        //        "ContosoStartupId",
        //        executablePath
        //    );
        //}

        //private void UnregisterButton_Click(object sender, RoutedEventArgs e)
        //{
        //    OutputMessage("Unregistering for rich activation");

        //    // BUG Unregistering a filetype removes the 
        //    // [HKEY_CURRENT_USER\Software\Classes\App.5761a1850f7033ad.File]
        //    // entry, and the value under [HKEY_CURRENT_USER\Software\Classes\.foo\OpenWithProgids]
        //    // but not the [HKEY_CURRENT_USER\Software\Classes\.foo] key.

        //    // Unregister one or more registered filetypes.
        //    try
        //    {
        //        string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
        //        ActivationRegistrationManager.UnregisterForFileTypeActivation(
        //            myFileTypes,
        //            executablePath
        //        );
        //    }
        //    catch (Exception ex)
        //    {
        //        OutputMessage($"Error unregistering file types {ex.Message}");
        //    }

        //    // BUG Unregistering a protocol removes the 
        //    // [HKEY_CURRENT_USER\Software\Classes\App.5761a1850f7033ad.Protocol]
        //    // entry, but not the [HKEY_CURRENT_USER\Software\Classes\foo] key.

        //    // Unregister a protocol scheme.
        //    ActivationRegistrationManager.UnregisterForProtocolActivation(
        //        "foo",
        //        "");

        //    // Unregister for startup activation.
        //    ActivationRegistrationManager.UnregisterForStartupActivation(
        //        "ContosoStartupId");
        //}

        // Use the Windows App SDK version of AppInstance::GetActivatedEventArgs.

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
