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
    internal class NotificationManager
    {
        private bool m_isRegistered;

        private Dictionary<string, Action<AppNotificationActivatedEventArgs>> c_map;

        public NotificationManager()
        {
            m_isRegistered = false;

            c_map = new Dictionary<string, Action<AppNotificationActivatedEventArgs>>();
            c_map.Add("1", ToastWithAvatar.NotificationReceived);
            c_map.Add("2", ToastWithTextBox.NotificationReceived);
        }

        ~NotificationManager()
        {
            Unregister();
        }

        public void Init()
        {
            AppNotificationManager notificationManager = AppNotificationManager.Default;

            notificationManager.NotificationInvoked += OnNotificationInvoked;

            notificationManager.Register();
            m_isRegistered = true;
        }

        public void Unregister()
        {
            if (m_isRegistered)
            {
                AppNotificationManager.Default.Unregister();
                m_isRegistered = false;
            }
        }

        public void ProcessLaunchActivationArgs(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            //assert(m_isRegistered);

            DispatchNotification(notificationActivatedEventArgs);
            MainPage.Current.NotifyUser("App launched from notifications", InfoBarSeverity.Informational);
        }

        public bool DispatchNotification(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
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

        void OnNotificationInvoked(object sender, AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            MainPage.Current.NotifyUser("Notification received", InfoBarSeverity.Informational);

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                MainPage.Current.NotifyUser("Unrecognized Toast Originator", InfoBarSeverity.Error);
            }
        }
    }
}
