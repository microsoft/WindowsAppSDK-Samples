// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using Microsoft.Windows.AppLifecycle;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using global::Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsWpfInstancing
{
    public class Program
    {
        // Windows App SDK version.
        private static uint majorMinorVersion = 0x00010000;
     
        private static string executablePath;
        private static string executablePathAndIconIndex;
        private static int activationCount = 1;
        public static List<string> OutputStack = new();

        // NOTE We can't make Main async in a WPF app.
        [STAThread]
        public static int Main(string[] args)
        {
            executablePath = Process.GetCurrentProcess().MainModule.FileName;
            executablePathAndIconIndex = $"{executablePath},0";

            // Initialize Windows App SDK for unpackaged apps.            
            int result = 0;
            if (Bootstrap.TryInitialize(majorMinorVersion, out result))
            {
                bool isRedirect = DecideRedirection();
                if (!isRedirect)
                {
                    App app = new()
                    {
                        StartupUri = new Uri("MainWindow.xaml", UriKind.Relative)
                    };
                    app.Run();
                }

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
            return 0;
        }

        private static void ReportInfo(string message)
        {
            // If we already have a form, display the message now.
            // Otherwise, add it to the collection for displaying later.
            if (Application.Current != null)
            {
                // Ensure we access the MainWindow on the correct thread,
                // in case this function is called from a different thread.
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    if (Application.Current.MainWindow is MainWindow mainWindow)
                    {
                        mainWindow.OutputMessage(message);
                    }
                }));
            }
            else
            {
                OutputStack.Add(message);
            }
        }

        // P/Invokes for creating and waiting on an event: we use these to
        // perform the async redirect call in a non-blocking way.
        [DllImport("kernel32.dll")]
        private static extern IntPtr CreateEvent(
            IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName);

        [DllImport("kernel32.dll")]
        private static extern bool SetEvent(IntPtr hEvent);

        [DllImport("ole32.dll")]
        private static extern uint CoWaitForMultipleObjects(
            uint dwFlags, uint dwMilliseconds, ulong nHandles, IntPtr[] pHandles, out uint dwIndex);

        private static IntPtr redirectEventHandle = IntPtr.Zero;

        private static bool DecideRedirection()
        {
            bool isRedirect = false;

            // Find out what kind of activation this is.
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            ReportInfo($"ActivationKind={kind}");
            if (kind == ExtendedActivationKind.Launch)
            {
                ReportLaunchArgs("Main", args);
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

                            // Ensure we don't block the STA, by doing the redirect operation
                            // in another thread, and using an event to signal when it has completed.
                            redirectEventHandle = CreateEvent(IntPtr.Zero, true, false, null);
                            if (redirectEventHandle != IntPtr.Zero)
                            {
                                Task.Run(() =>
                                {
                                    keyInstance.RedirectActivationToAsync(args).AsTask().Wait();
                                    SetEvent(redirectEventHandle);
                                });
                                uint CWMO_DEFAULT = 0;
                                uint INFINITE = 0xFFFFFFFF;
                                _ = CoWaitForMultipleObjects(
                                    CWMO_DEFAULT, INFINITE, 1, 
                                    new IntPtr[] { redirectEventHandle }, out uint handleIndex);
                            }
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
                    if (!string.IsNullOrWhiteSpace(argString))
                    {
                        ReportInfo($"arg[{i}] = {argString}");
                    }
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
