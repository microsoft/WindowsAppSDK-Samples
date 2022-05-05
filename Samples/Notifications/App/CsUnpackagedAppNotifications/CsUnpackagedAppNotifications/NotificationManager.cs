using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Windows.AppNotifications;
using Windows.Foundation;
using Microsoft.UI.Xaml.Controls;

namespace CsUnpackagedAppNotifications
{
    internal static class NotificationManager
    {
        private static readonly Destructor Finalise = new Destructor();
        private static bool m_isRegistered;

        private static Dictionary<string, Action<AppNotificationActivatedEventArgs>> c_map = new Dictionary<string, Action<AppNotificationActivatedEventArgs>>();

        public sealed class Destructor
        {
            ~Destructor()
            {
                if (m_isRegistered)
                {
                    AppNotificationManager.Default.Unregister();
                }
            }
        }

        static NotificationManager()
        {
            m_isRegistered = false;

            c_map.Add("1", ToastWithAvatar.NotificationReceived);
            c_map.Add("2", ToastWithTextBox.NotificationReceived);
        }
#if false
        ~NotificationManager()
        {
            if (m_isRegistered)
            {
                AppNotificationManager.Default.Unregister();
            }
        }
#endif
        public static void Init()
        {
            AppNotificationManager notificationManager = AppNotificationManager.Default;

            //const TypedEventHandler<AppNotificationManager, AppNotificationActivatedArgumentArgs> token = 
            notificationManager.NotificationInvoked += MyEvent;

            notificationManager.Register();
            m_isRegistered = true;
        }

        public static void ProcessLaunchActivationArgs(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            //assert(m_isRegistered);

            DispatchNotification(notificationActivatedEventArgs);
            MainPage.Current.NotifyUser("App launched from notifications", InfoBarSeverity.Informational);
        }

        public static bool DispatchNotification(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            string scenarioId = Common.ExtractParam(notificationActivatedEventArgs.Argument, "scenarioId");
            if (scenarioId == null)
            {
                return false;
            }

            try
            {
                c_map[scenarioId](notificationActivatedEventArgs);
                return true;
            }
            catch
            {
                return false;
            }
        }

        static void MyEvent(object sender, AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            MainPage.Current.NotifyUser("Notification received", InfoBarSeverity.Informational);

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                MainPage.Current.NotifyUser("Unrecognized Toast Originator", InfoBarSeverity.Error);
            }
        }
    }
}
