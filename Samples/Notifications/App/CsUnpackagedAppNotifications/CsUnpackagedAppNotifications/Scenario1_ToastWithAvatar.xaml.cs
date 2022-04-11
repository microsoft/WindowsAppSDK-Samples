// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

using Microsoft.Windows.AppNotifications;

namespace CsUnpackagedAppNotifications
{
    public partial class Scenario1_ToastWithAvatar : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_ToastWithAvatar()
        {
            this.InitializeComponent();
        }

        private void SendToast_Click(object sender, RoutedEventArgs e)
        {
            var xmlPayload =
                "<toast><visual><binding template =\"ToastGeneric\"><text>App Notifications Sample Scenario 1</text><text> This is an example message using XML</text></binding></visual></toast>";

            var appNotificationManager = AppNotificationManager.Default;

            AppNotification appNotification = new AppNotification(xmlPayload);
            appNotification.Priority = AppNotificationPriority.High;

            appNotificationManager.Register();
            appNotificationManager.Show(appNotification);


            /*        if (toast.Id() == 0)
                    {
                        rootPage.NotifyUser("Could not send toast", InfoBarSeverity.Error);
                    } */

            rootPage.NotifyUser("Toast sent successfully!", InfoBarSeverity.Success);
        }
    }
}
