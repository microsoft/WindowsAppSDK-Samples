// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using CsUnpackagedAppNotifications;
using Microsoft.Windows.AppNotifications;
using Microsoft.Windows.AppNotifications.Builder;

class ToastWithTextBox
{
    public const int ScenarioId = 2;
    public const string ScenarioName = "Local Toast with Avatar and Text Box";
    const string textboxReplyId = "textboxReply";

    public static bool SendToast()
    {
        var appNotification = new AppNotificationBuilder()
            .AddArgument("action", "ToastClick")
            .AddArgument(Common.scenarioTag, ScenarioId.ToString())
            .SetAppLogoOverride(new System.Uri("file://" + App.GetFullPathToAsset("Square150x150Logo.png")), AppNotificationImageCrop.Circle)
            .AddText(ScenarioName)
            .AddText("This is an example message using XML")
            .AddTextBox(textboxReplyId, "Type a reply", "Reply box")
            .AddButton(new AppNotificationButton("Reply")
                .AddArgument("action", "Reply")
                .AddArgument(Common.scenarioTag, ScenarioId.ToString())
                .SetInputId(textboxReplyId))
            .BuildNotification();

        AppNotificationManager.Default.Show(appNotification);

        return appNotification.Id != 0; // return true (indicating success) if the toast was sent (if it has an Id)
    }

    public static void NotificationReceived(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
    {
        // In a real-life scenario, this type of action would usually be processed in the background. Your App would process the payload in
        // the background (sending the payload back to your App Server) without ever showing the App's UI.
        // This is not something that can easily be demonstrated in a sample such as this one, as we need to show the UI to demonstrate how
        // the payload is routed internally
        
        var notification = new MainPage.Notification();
        notification.Originator = ScenarioName;
        notification.Action = notificationActivatedEventArgs.Arguments["action"];
        notification.HasInput = true;
        notification.Input = notificationActivatedEventArgs.UserInput[textboxReplyId];
        MainPage.Current.NotificationReceived(notification);
        App.ToForeground();
    }
}
