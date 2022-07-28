
namespace CsUnpackagedAppNotifications
{
    using Microsoft.UI.Xaml.Controls;

    class NotifyUser
    {
        public static void CouldNotSendToast()
        {
            MainPage.Current.NotifyUser("Could not send toast", InfoBarSeverity.Error);
        }

        public static void ToastSentSuccessfully()
        {
            MainPage.Current.NotifyUser("Toast sent successfully!", InfoBarSeverity.Success);
        }

        public static void AppLaunchedFromNotification()
        {
            MainPage.Current.NotifyUser("App launched from notifications", InfoBarSeverity.Informational);
        }

        public static void NotificationReceived()
        {
            MainPage.Current.NotifyUser("Notification received", InfoBarSeverity.Informational);
        }

        public static void UnrecognizedToastOriginator()
        {
            MainPage.Current.NotifyUser("Unrecognized Toast Originator", InfoBarSeverity.Error);
        }
    }
}