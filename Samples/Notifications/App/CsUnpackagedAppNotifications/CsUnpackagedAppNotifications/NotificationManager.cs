using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Windows.AppNotifications;
using Windows.Foundation;

namespace CsUnpackagedAppNotifications
{
    internal static class NotificationManager
    {
        private static readonly Destructor Finalise = new Destructor();
        private static bool m_isRegistered;

        private sealed class Destructor
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

            //notificationManager.Register();
            m_isRegistered = true;
        }
#if false
    void ProcessLaunchActivationArgs(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
    {
        assert(m_isRegistered);

        DispatchNotification(notificationActivatedEventArgs);
        winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"App launched from notifications", winrt::InfoBarSeverity::Informational);
    }

    bool DispatchNotification(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
    {
        auto scenarioId{ Common::ExtractParam(notificationActivatedEventArgs.Argument().c_str(), L"scenarioId") };
        if (scenarioId.has_value())
        {
            try
            {
                c_map.at(scenarioId.value())(notificationActivatedEventArgs); // Will throw if out of range
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
#endif
        static void MyEvent(object sender, AppNotificationActivatedEventArgs notificationActivatedEventArgs)
                {
#if false
                winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Notification received", winrt::InfoBarSeverity::Informational);

                    if (!DispatchNotification(notificationActivatedEventArgs))
                    {
                        winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", winrt::InfoBarSeverity::Error);
                    }
#endif
                }
}
}
