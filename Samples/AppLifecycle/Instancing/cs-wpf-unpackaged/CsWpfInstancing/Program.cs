// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.Windows.AppLifecycle;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Activation;
using Windows.Storage;

namespace CsWpfInstancing
{
    public class Program
    {
        private static uint majorMinorVersion = 0x00010000;
        private static string versionTag = "preview1";

        private static string executablePath;
        private static string executablePathAndIconIndex;
        private static int activationCount = 1;
        public static List<string> OutputStack = new();

        [STAThread]
        static void Main(string[] args)
        {
            executablePath = Process.GetCurrentProcess().MainModule.FileName;
            executablePathAndIconIndex = $"{executablePath},1";

            // Initialize WASDK for unpackaged apps.            
            int result = MddBootstrap.Initialize(majorMinorVersion, versionTag);
            if (result == 0)
            {
                Task<bool> task = DetermineActivationKind();
                task.Wait();
                bool isRedirect = task.Result;
                if (!isRedirect)
                {
                    App app = new()
                    {
                        StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
                    };
                    app.Run();
                }

                // Uninitialize WASDK.
                MddBootstrap.Shutdown();
            }
        }

        private static void ReportInfo(string message)
        {
            // If we already have a form, display the message now.
            // Otherwise, add it to the collection for displaying later.
            if (App.Current != null && App.Current.MainWindow != null)
            {
                if (App.Current.MainWindow is MainWindow mainWindow)
                {
                    mainWindow.OutputMessage(message);
                }
            }
            else
            {
                OutputStack.Add(message);
            }
        }

        private static async Task<bool> DetermineActivationKind()
        {
            bool isRedirect = false;

            // Find out what kind of activation this is.
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            ReportInfo($"ActivationKind={kind}");
            if (kind == ExtendedActivationKind.Launch)
            {
                // This is a launch activation: here we'll register for file activation.
                ReportLaunchArgs("Main", args);
                RegisterForFileActivation();
            }
            else if (kind == ExtendedActivationKind.File)
            {
                ReportFileArgs("Main", args);

                try
                {
                    // This is a file activation: here we'll get the file information,
                    // and register the file name as our instance key.
                    if (args.Data is IFileActivatedEventArgs fileArgs)
                    {
                        IStorageItem file = fileArgs.Files[0];
                        AppInstance keyInstance = AppInstance.FindOrRegisterForKey(file.Name);
                        ReportInfo($"Registered key = {keyInstance.Key}");

                        // If we successfully registered the file name, we must be the
                        // only instance running that was activated for this file.
                        if (keyInstance.IsCurrent)
                        {
                            // Report successful file name key registration.
                            ReportInfo($"IsCurrent=true; registered this instance for {file.Name}");

                            // Hook up the Activated event, to allow for this instance of the app
                            // getting reactivated as a result of multi-instance redirection.
                            keyInstance.Activated += OnActivated;
                        }
                        else
                        {
                            isRedirect = true;
                            await keyInstance.RedirectActivationToAsync(args);
                        }
                    }
                }
                catch (Exception ex)
                {
                    ReportInfo($"Error getting instance information: {ex.Message}");
                }
            }

            return isRedirect;
        }

        private static void ReportFileArgs(string callSite, AppActivationArguments args)
        {
            ReportInfo($"called from {callSite}");
            if (args.Data is IFileActivatedEventArgs fileArgs)
            {
                IStorageItem item = fileArgs.Files.FirstOrDefault();
                if (item is StorageFile file)
                {
                    ReportInfo($"file: {file.Name}");
                }
            }
        }

        private static void ReportLaunchArgs(string callSite, AppActivationArguments args)
        {
            ReportInfo($"called from {callSite}");
            if (args.Data is ILaunchActivatedEventArgs launchArgs)
            {
                string[] argStrings = launchArgs.Arguments.Split();
                for (int i = 0; i < argStrings.Length; i++)
                {
                    string argString = argStrings[i];
                    ReportInfo($"arg[{i}] = {argString}");
                }
            }
        }

        private static void OnActivated(object sender, AppActivationArguments args)
        {
            ExtendedActivationKind kind = args.Kind;
            if (kind == ExtendedActivationKind.Launch)
            {
                ReportLaunchArgs($"OnActivated ({activationCount++})", args);
            }
            else if (kind == ExtendedActivationKind.File)
            {
                ReportFileArgs($"OnActivated ({activationCount++})", args);
            }
        }

        public static void GetActivationInfo()
        {
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            ReportInfo($"ActivationKind: {kind}");

            if (kind == ExtendedActivationKind.Launch)
            {
                if (args.Data is ILaunchActivatedEventArgs launchArgs)
                {
                    string argString = launchArgs.Arguments;
                    string[] argStrings = argString.Split();
                    foreach (string arg in argStrings)
                    {
                        if (!string.IsNullOrWhiteSpace(arg))
                        {
                            ReportInfo(arg);
                        }
                    }
                }
            }
            else if (kind == ExtendedActivationKind.File)
            {
                if (args.Data is IFileActivatedEventArgs fileArgs)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    if (file != null)
                    {
                        ReportInfo(file.Name);
                    }
                }
            }
        }

        public static void RegisterForFileActivation()
        {
            // Register one or more supported filetypes, 
            // an icon (specified by binary file path plus resource index),
            // a display name to use in Shell and Settings,
            // zero or more verbs for the File Explorer context menu,
            // and the path to the EXE to register for activation.
            string[] myFileTypes = { ".moo" };
            string[] verbs = { "view", "edit" };
            ActivationRegistrationManager.RegisterForFileTypeActivation(
                myFileTypes,
                executablePathAndIconIndex,
                "Contoso File Types",
                verbs,
                executablePath
            );
        }

        public static void UnregisterForFileActivation()
        {
            // BUG Unregistering a filetype removes the 
            // [HKEY_CURRENT_USER\Software\Classes\App.5761a1850f7033ad.File]
            // entry, and the value under [HKEY_CURRENT_USER\Software\Classes\.foo\OpenWithProgids]
            // but not the [HKEY_CURRENT_USER\Software\Classes\.foo] key.

            // Unregister one or more registered filetypes.
            try
            {
                string[] myFileTypes = { ".moo" };
                ActivationRegistrationManager.UnregisterForFileTypeActivation(
                    myFileTypes,
                    executablePath
                );
            }
            catch (Exception ex)
            {
                ReportInfo($"Error unregistering file types {ex.Message}");
            }
        }

    }
}
