// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.AppLifecycle;
using Microsoft.UI.Xaml;

namespace CsUnpackagedAppNotifications
{

    public partial class App : Application
    {
        private Window mainWindow;
        //private NotificationManager notificationManager;

        public App()
        {
            //notificationManager = new NotificationManager();
            this.InitializeComponent();
        }

    public static string GetFullPathToExe()
        {
            //TCHAR buffer[MAX_PATH] = { 0 };
            //GetModuleFileName(NULL, buffer, MAX_PATH);
            //std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
            return ""; //std::wstring(buffer).substr(0, pos);
        }

        public static string GetFullPathToAsset(string assetName)
        {
            return GetFullPathToExe() + "\\Assets\\" + assetName;
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            mainWindow = new MainWindow();

            NotificationManager.Init();

            // NOTE: AppInstance is ambiguous between
            // Microsoft.Windows.AppLifecycle.AppInstance and
            // Windows.ApplicationModel.AppInstance
            AppInstance currentInstance = AppInstance.GetCurrent();
            if (currentInstance.IsCurrent)
            {
                // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
                // even in WinUI's OnLaunched.
                AppActivationArguments activationArgs = currentInstance.GetActivatedEventArgs();
                //if (activationArgs)
                {
                    ExtendedActivationKind extendedKind = activationArgs.Kind;
                    if (extendedKind == ExtendedActivationKind.AppNotification)
                    {
                        //AppNotificationActivatedEventArgs notificationActivatedEventArgs = activationArgs.Data().as< winrt::AppNotificationActivatedEventArgs > ();
                        //g_notificationManager.ProcessLaunchActivationArgs(notificationActivatedEventArgs);
                    }
                }
            }

            mainWindow.Activate();
        }
    }
}
