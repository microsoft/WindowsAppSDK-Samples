// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.AppLifecycle;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppNotifications;
using Windows.Win32.Foundation;
using System;
using CsUnpackagedAppNotifications.Notifications;
using static Windows.Win32.PInvoke;

namespace CsUnpackagedAppNotifications
{

    public partial class App : Application
    {
        private static Window mainWindow = null;
        private NotificationManager notificationManager;

        public App()
        {
            notificationManager = new NotificationManager();
            notificationManager.Init();

            this.InitializeComponent();
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);
        }

    	public static void ToForeground()
    	{
            if (mainWindow != null)
            {
                HWND hwnd = (HWND)WinRT.Interop.WindowNative.GetWindowHandle(mainWindow);
                SwitchToThisWindow(hwnd, true);
            }
        }
	
        public static string GetFullPathToExe()
        {
            var path = AppDomain.CurrentDomain.BaseDirectory;
            var pos = path.LastIndexOf("\\");
            return path.Substring(0, pos);
        }

        public static string GetFullPathToAsset(string assetName)
        {
            return GetFullPathToExe() + "\\Assets\\" + assetName;
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            mainWindow = new MainWindow();

            // NOTE: AppInstance is ambiguous between
            // Microsoft.Windows.AppLifecycle.AppInstance and
            // Windows.ApplicationModel.AppInstance
            var currentInstance = AppInstance.GetCurrent();
            if (currentInstance.IsCurrent)
            {
                // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
                // even in WinUI's OnLaunched.
                AppActivationArguments activationArgs = currentInstance.GetActivatedEventArgs();
                if (activationArgs != null)
                {
                    ExtendedActivationKind extendedKind = activationArgs.Kind;
                    if (extendedKind == ExtendedActivationKind.AppNotification)
                    {
                        var notificationActivatedEventArgs = (AppNotificationActivatedEventArgs)activationArgs.Data;
                        notificationManager.ProcessLaunchActivationArgs(notificationActivatedEventArgs);
                    }
                }
            }

            mainWindow.Activate();
        }

        void OnProcessExit(object sender, EventArgs e)
        {
            notificationManager.Unregister();
        }
    }
}
