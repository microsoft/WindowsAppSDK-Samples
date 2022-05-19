﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Windows.AppNotifications;
using Windows.Foundation;
using Microsoft.UI.Xaml.Controls;

namespace CsUnpackagedAppNotifications.Notifications
{
    internal class NotificationManager
    {
        private bool m_isRegistered;

        private Dictionary<int, Action<AppNotificationActivatedEventArgs>> c_notificationHandlers;

        public NotificationManager()
        {
            m_isRegistered = false;

            // When adding new a scenario, be sure to add its notification handler here.
            c_notificationHandlers = new Dictionary<int, Action<AppNotificationActivatedEventArgs>>();
            c_notificationHandlers.Add(ToastWithAvatar.ScenarioId, ToastWithAvatar.NotificationReceived);
            c_notificationHandlers.Add(ToastWithTextBox.ScenarioId, ToastWithTextBox.NotificationReceived);
        }

        ~NotificationManager()
        {
            Unregister();
        }

        public void Init()
        {
            var notificationManager = AppNotificationManager.Default;

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
            NotifyUser.AppLaunchedFromNotification();
        }

        public bool DispatchNotification(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            var scenarioId = Common.ExtractScenarioIdFromArgs(notificationActivatedEventArgs.Argument);
            if (scenarioId == 0)
            {
                return false;
            }

            try
            {
                c_notificationHandlers[scenarioId](notificationActivatedEventArgs);
                return true;
            }
            catch
            {
                return false;
            }
        }

        void OnNotificationInvoked(object sender, AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            NotifyUser.NotificationReceived();

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                NotifyUser.UnrecognizedToastOriginator();
            }
        }
    }
}
