// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using CsUnpackagedAppNotifications;
using Microsoft.Windows.AppNotifications;

class ToastWithTextBox
{
    public const int ScenarioId = 2;
    public const string ScenarioName = "Local Toast with Avatar and Text Box";
    const string textboxReplyId = "textboxReply";

    public static bool SendToast()
    {
        // The ScenarioIdToken uniquely identify a scenario and is used to route the response received when the user clicks on a toast to the correct scenario.
        var ScenarioIdToken = Common.MakeScenarioIdToken(ScenarioId);

        var xmlPayload = new string(
        	"<toast launch = \"action=ToastClick&amp;" + ScenarioIdToken + "\">"
        +       "<visual>"
        +           "<binding template = \"ToastGeneric\">"
        +               "<image placement = \"appLogoOverride\" hint-crop=\"circle\" src = \"" + App.GetFullPathToAsset("Square150x150Logo.png") + "\"/>"
        +               "<text>" + ScenarioName + "</text>"
        +               "<text>This is an example message using XML</text>"
        +           "</binding>"
        +       "</visual>"
        +       "<actions>"
        +           "<input "
        +               "id = \"" + textboxReplyId + "\" "
        +               "type = \"text\" "
        +               "placeHolderContent = \"Type a reply\"/>"
        +           "<action "
        +               "content = \"Reply\" "
        +               "arguments = \"action=Reply&amp;" + ScenarioIdToken + "\" "
        +               "hint-inputId = \"" + textboxReplyId + "\"/>"
        +       "</actions>"
        +   "</toast>");

        var toast = new AppNotification(xmlPayload);
        AppNotificationManager.Default.Show(toast);
 
    	return toast.Id != 0; // return true (indicating success) if the toast was sent (if it has an Id)
    }

    public static void NotificationReceived(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
    {
    // In a real-life scenario, this type of action would usually be processed in the background. Your App would process the payload in
    // the background (sending the payload back to your App Server) without ever showing the App's UI.
    // This is not something that can easily be demonstrated in a sample such as this one, as we need to show the UI to demonstrate how
    // the payload is routed internally
        var input = notificationActivatedEventArgs.UserInput;
        var text = input[textboxReplyId];

        var notification = new MainPage.Notification();
        notification.Originator = ScenarioName;
        var action = Common.ExtractParamFromArgs(notificationActivatedEventArgs.Argument, "action");
        notification.Action = action == null ? "" : action;
        notification.HasInput = true;
        notification.Input = text;
        MainPage.Current.NotificationReceived(notification);
        App.ToForeground();
    }
}
