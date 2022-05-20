// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using CsUnpackagedAppNotifications;
using Microsoft.Windows.AppNotifications;

class ToastWithAvatar
{
    public const int ScenarioId = 1;
    public const string ScenarioName = "Local Toast with Avatar Image";

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
        +           "<action "
        +               "content = \"Open App\" "
        +               "arguments = \"action=OpenApp&amp;" + ScenarioIdToken + "\"/>"
        +       "</actions>"
        +   "</toast>" );

        var toast = new AppNotification(xmlPayload);
        AppNotificationManager.Default.Show(toast);

    	return toast.Id != 0; // return true (indicating success) if the toast was sent (if it has an Id)

    }

    public static void NotificationReceived(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
    {
        var notification = new MainPage.Notification();
        notification.Originator = ScenarioName;
        var action = Common.ExtractParamFromArgs(notificationActivatedEventArgs.Argument, "action");
        notification.Action = action == null ? "" : action;
        MainPage.Current.NotificationReceived(notification);
        App.ToForeground();
    }
}
