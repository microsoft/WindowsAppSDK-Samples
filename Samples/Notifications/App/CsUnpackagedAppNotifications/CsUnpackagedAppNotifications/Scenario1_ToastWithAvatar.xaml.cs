// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

using Microsoft.Windows.AppNotifications;
using Windows.Win32;

namespace CsUnpackagedAppNotifications
{
    public partial class Scenario1_ToastWithAvatar : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_ToastWithAvatar()
        {
            this.InitializeComponent();

            PInvoke.SetCurrentProcessExplicitAppUserModelID("{9E4FC10C-12DF-49C5-8756-E87DEB7F21D5}");

            var appNotificationManager = AppNotificationManager.Default;

            appNotificationManager.Register();
        }

        private void SendToast_Click(object sender, RoutedEventArgs e)
        {
            var xmlPayload =
                "<toast><visual><binding template =\"ToastGeneric\"><text>App Notifications Sample Scenario 1</text><text> This is an example message using XML</text></binding></visual></toast>";

            AppNotification appNotification = new AppNotification(xmlPayload);
            appNotification.Priority = AppNotificationPriority.High;

            var appNotificationManager = AppNotificationManager.Default;
            appNotificationManager.Show(appNotification);

            /*        if (toast.Id() == 0)
                    {
                        rootPage.NotifyUser("Could not send toast", InfoBarSeverity.Error);
                    } */

            rootPage.NotifyUser("Toast sent successfully!", InfoBarSeverity.Success);
        }
    }
}
