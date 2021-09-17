﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.Windows.AppLifecycle;
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using static CsConsoleInstancing.NativeHelpers;

namespace CsConsoleInstancing
{
    class Program
    {
        private static uint majorMinorVersion = 0x00010000;
        private static string versionTag = "preview1";

        private static string executablePath;
        private static string executablePathAndIconIndex;
        private static int activationCount = 1;

        static void Main(string[] args)
        {
            // Initialize WASDK for unpackaged apps.            
            int result = MddBootstrap.Initialize(majorMinorVersion, versionTag);
            if (result == 0)
            {
                StringBuilder builder = new(MAX_PATH);
                GetModuleFileName(IntPtr.Zero, builder, builder.Capacity);
                executablePath = builder.ToString();
                executablePathAndIconIndex = $"{executablePath},1";

                Task<bool> task = DetermineActivationKind();
                task.Wait();
                bool isRedirect = task.Result;
                if (!isRedirect)
                {
                    // Simulate doing work here, while we wait for
                    // later activations.
                    for (int i = 0; i < 20; i++)
                    {
                        Console.WriteLine($"processing ({i})...");
                        Task.Delay(1000).Wait();
                    }
                }

                // Uninitialize WASDK.
                MddBootstrap.Shutdown();
            }

            // Pause long enough to show the Closing message.
            Console.WriteLine("Closing...");
            Task.Delay(3000);
        }

        private static void ReportInfo(string message)
        {
            Console.WriteLine(message);
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
