// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

using System;
using System.Text;
using System.Linq;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using Microsoft.Windows.AppLifecycle;
using static CsConsoleActivation.NativeHelpers;
using global::Microsoft.Windows.ApplicationModel.DynamicDependency;

namespace CsConsoleActivation
{
    class Program
    {
        // Windows App SDK version.
        private static uint majorMinorVersion = 0x00010000;

        private static string executablePath;
        private static string executablePathAndIconIndex;

        static void Main(string[] args)
        {
            // Initialize Windows App SDK for unpackaged apps.            
            int result = 0;
            if (Bootstrap.TryInitialize(majorMinorVersion, out result))
            {
                StringBuilder builder = new StringBuilder(MAX_PATH);
                GetModuleFileName(IntPtr.Zero, builder, builder.Capacity);
                executablePath = builder.ToString();
                executablePathAndIconIndex = $"{executablePath},0";
                Console.WriteLine($"Exe: {executablePathAndIconIndex}");

                int choice = 0;
                while (choice != 4)
                {
                    Console.WriteLine("\nMENU");
                    Console.WriteLine("1 - Get activation info");
                    Console.WriteLine("2 - Register for rich activation");
                    Console.WriteLine("3 - Unregister for rich activation");
                    Console.WriteLine("4 - Quit");
                    Console.WriteLine("Select an option: ");
                    if (int.TryParse(Console.ReadLine(), out int tmp))
                    {
                        choice = tmp;
                        switch (choice)
                        {
                            case 1:
                                GetActivationInfo();
                                break;
                            case 2:
                                RegisterForActivation();
                                break;
                            case 3:
                                UnregisterForActivation();
                                break;
                            case 4:
                                break;
                            default:
                                Console.WriteLine($"*** Error: {choice} is not a valid choice ***");
                                break;
                        }
                    }
                }

                // Uninitialize Windows App SDK.
                Bootstrap.Shutdown();
            }
        }

        private static void OutputMessage(string message)
        {
            Console.WriteLine(message);
        }

        private static void RegisterForActivation()
        {
            // Register one or more supported filetypes, 
            // an icon (specified by binary file path plus resource index),
            // a display name to use in Shell and Settings,
            // zero or more verbs for the File Explorer context menu,
            // and the path to the EXE to register for activation.
            string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
            string[] verbs = { "view", "edit" };
            ActivationRegistrationManager.RegisterForFileTypeActivation(
                myFileTypes,
                executablePathAndIconIndex,
                "Contoso File Types",
                verbs,
                executablePath
            );

            // Register a URI scheme for protocol activation,
            // specifying the scheme name, icon, display name and EXE path.
            ActivationRegistrationManager.RegisterForProtocolActivation(
                "foo",
                executablePathAndIconIndex,
                "Contoso Foo Protocol",
                executablePath
            );

            // Register for startup activation.
            ActivationRegistrationManager.RegisterForStartupActivation(
                "ContosoStartupId",
                executablePath
            );

            OutputMessage("Registered for rich activation");
        }

        private static void UnregisterForActivation()
        {
            // BUG Unregistering a filetype removes the 
            // [HKEY_CURRENT_USER\Software\Classes\App.5761a1850f7033ad.File]
            // entry, and the value under [HKEY_CURRENT_USER\Software\Classes\.foo\OpenWithProgids]
            // but not the [HKEY_CURRENT_USER\Software\Classes\.foo] key.

            // Unregister one or more registered filetypes.
            try
            {
                string[] myFileTypes = { ".foo", ".foo2", ".foo3" };
                ActivationRegistrationManager.UnregisterForFileTypeActivation(
                    myFileTypes,
                    executablePath
                );
            }
            catch (Exception ex)
            {
                OutputMessage($"Error unregistering file types {ex.Message}");
            }

            // BUG Unregistering a protocol removes the 
            // [HKEY_CURRENT_USER\Software\Classes\App.5761a1850f7033ad.Protocol]
            // entry, but not the [HKEY_CURRENT_USER\Software\Classes\foo] key.

            // Unregister a protocol scheme.
            ActivationRegistrationManager.UnregisterForProtocolActivation(
                "foo",
                "");

            // Unregister for startup activation.
            ActivationRegistrationManager.UnregisterForStartupActivation(
                "ContosoStartupId");

            OutputMessage("Unregistered for rich activation");
        }

        private static void GetActivationInfo()
        {
            AppActivationArguments args = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = args.Kind;
            OutputMessage($"ActivationKind: {kind}");

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
                            OutputMessage(arg);
                        }
                    }
                }
            }
            else if (kind == ExtendedActivationKind.File)
            {
                if (args.Data is IFileActivatedEventArgs fileArgs)
                {
                    IStorageItem file = fileArgs.Files.FirstOrDefault();
                    OutputMessage(file.Name);
                }
            }
            else if (kind == ExtendedActivationKind.Protocol)
            {
                if (args.Data is IProtocolActivatedEventArgs protocolArgs)
                {
                    Uri uri = protocolArgs.Uri;
                    OutputMessage(uri.AbsoluteUri);
                }
            }
            else if (kind == ExtendedActivationKind.StartupTask)
            {
                if (args.Data is IStartupTaskActivatedEventArgs startupArgs)
                {
                    OutputMessage(startupArgs.TaskId);
                }
            }
        }
    }
}
