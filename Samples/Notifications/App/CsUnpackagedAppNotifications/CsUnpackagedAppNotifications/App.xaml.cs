// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;

namespace CsUnpackagedAppNotifications
{
    public partial class App : Application
    {
        private Window mainWindow;

        public App()
        {
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
            return ""; //etFullPathToExe() + L"\\Assets\\" + assetName;
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            mainWindow = new MainWindow();
#if false
        g_notificationManager.Init();

        // NOTE: AppInstance is ambiguous between
        // Microsoft.Windows.AppLifecycle.AppInstance and
        // Windows.ApplicationModel.AppInstance
        auto currentInstance{ winrt::AppInstance::GetCurrent() };
        if (currentInstance)
        {
            // AppInstance.GetActivatedEventArgs will report the correct ActivationKind,
            // even in WinUI's OnLaunched.
            winrt::AppActivationArguments activationArgs{ currentInstance.GetActivatedEventArgs() };
            if (activationArgs)
            {
                winrt::ExtendedActivationKind extendedKind{ activationArgs.Kind() };
                if (extendedKind == winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind::AppNotification)
                {
                    winrt::AppNotificationActivatedEventArgs notificationActivatedEventArgs{ activationArgs.Data().as<winrt::AppNotificationActivatedEventArgs>() };
                    g_notificationManager.ProcessLaunchActivationArgs(notificationActivatedEventArgs);
                }


            }
        }
#endif
            mainWindow.Activate();
        }
    }
}
