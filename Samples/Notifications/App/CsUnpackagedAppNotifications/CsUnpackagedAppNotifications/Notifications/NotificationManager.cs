// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Microsoft.Windows.AppNotifications;

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

            // To ensure all Notification handling happens in this process instance, register for
            // NotificationInvoked before calling Register(). Without this a new process will
            // be launched to handle the notification.
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
            DispatchNotification(notificationActivatedEventArgs);
            NotifyUser.AppLaunchedFromNotification();
        }

        public bool DispatchNotification(AppNotificationActivatedEventArgs notificationActivatedEventArgs)
        {
            var scenarioId = notificationActivatedEventArgs.Arguments[Common.scenarioTag];
            if (scenarioId.Length != 0)
            {
                try
                {
                    c_notificationHandlers[int.Parse(scenarioId)](notificationActivatedEventArgs);
                    return true;
                }
                catch
                {
                    return false; // Couldn't find a NotificationHandler for scenarioId.
                }
            }
            else
            {
                return false; // No scenario specified in the notification
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
