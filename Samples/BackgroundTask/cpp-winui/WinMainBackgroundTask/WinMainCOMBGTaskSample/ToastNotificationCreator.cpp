#include "pch.h"
#include "ToastNotificationCreator.h"

using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::UI::Notifications;
namespace winrt::WinMainCOMBGTaskSample {
    ToastNotificationCreator::ToastNotificationCreator(bool isBGTask)
    {
        // Create the toast XML template
        XmlDocument toastXml;
        if (isBGTask)
        {
            toastXml.LoadXml(LR"(
                <toast>
                    <visual>
                        <binding template="ToastGeneric">
                            <text>Notification from BG Task</text>
                            <text>Run executed successfully!</text>
                        </binding>
                    </visual>
                </toast>)");
        }
        else
        {
            toastXml.LoadXml(LR"(
                <toast>
                    <visual>
                        <binding template="ToastGeneric">
                            <text>Notification from WinMainCOMBGTaskSample</text>
                            <text>Delete this notification to trigger background task.</text>
                        </binding>
                    </visual>
                </toast>)");
        }

        // Create the toast notification
        ToastNotification toast(toastXml);

        // Create a ToastNotifier and show the toast
        ToastNotificationManager::CreateToastNotifier().Show(toast);
    }
}