// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace CsUnpackagedAppNotifications
{
    public partial class Scenario2_ToastWithTextBox : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_ToastWithTextBox()
        {
            this.InitializeComponent();
        }

        private void SendToast_Click(object sender, RoutedEventArgs e)
        {
#if false
            winrt::hstring xmlPayload{
                "<toast>\
                    <visual>\
                        <binding template=\"ToastGeneric\">\
                            <text>App Notifications Sample Scenario 1</text>\
                            <text>This is an example message using XML</text>\
                        </binding>\
                    </visual>\
                    <actions>\
                        <action\
                            content = \"Activate Toast\"\
                            arguments = \"action=viewDetails&amp;contentId=351\"\
                            activationType = \"foreground\" />\
                    </actions>\
                </toast>" };

            auto toast{ winrt::AppNotification(xmlPayload) };
            toast.Priority(winrt::AppNotificationPriority::High);
            winrt::AppNotificationManager::Default().Show(toast);
            if (toast.Id() == 0)
            {
                rootPage.NotifyUser("Could not send toast", InfoBarSeverity.Error);
            }
#endif
            rootPage.NotifyUser("Toast sent successfully!", InfoBarSeverity.Success);
        }
    }
}
