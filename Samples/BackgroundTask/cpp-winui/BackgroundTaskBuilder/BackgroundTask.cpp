// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "BackgroundTask.h"
#include "winrt/Windows.Data.Xml.Dom.h"
#include "winrt/Windows.UI.Notifications.h"

using namespace winrt;
namespace winrt
{
    using namespace winrt::Windows::ApplicationModel::Background;
    using namespace Windows::Data::Xml::Dom;
    using namespace Windows::UI::Notifications;
}

namespace winrt::BackgroundTaskBuilder
{
    void BackgroundTask::Run(_In_ IBackgroundTaskInstance taskInstance)
    {
        // Get the deferral object from the task instance
        TaskDeferral = taskInstance.GetDeferral();
        taskInstance.Canceled({ this, &BackgroundTask::OnCanceled });
        // Create a toast XML template
        XmlDocument toastXml;
        toastXml.LoadXml(LR"(
            <toast>
                <visual>
                    <binding template="ToastGeneric">
                        <text>Notification: BackgroundTaskBuilder</text>
                        <text>Background Task triggered</text>
                    </binding>
                </visual>
            </toast>)");
        // Create the toast notification
        ToastNotification toast(toastXml);
        // Create a ToastNotifier and show the toast
        ToastNotificationManager::CreateToastNotifier().Show(toast);
        // Inform the system that the background task is completed
        TaskDeferral.Complete();
    }

    void BackgroundTask::CreateNotification()
    {
        // Create the toast XML template
        XmlDocument toastXml;
        toastXml.LoadXml(LR"(
        <toast>
            <visual>
                <binding template="ToastGeneric">
                    <text>Notification: BackgroundTaskBuilder</text>
                    <text>Change system timezone to trigger Background Task</text>
                </binding>
            </visual>
        </toast>)");
        // Create the toast notification
        ToastNotification toast(toastXml);
        // Create a ToastNotifier and show the toast
        ToastNotificationManager::CreateToastNotifier().Show(toast);
    }

    void BackgroundTask::OnCanceled(_In_ IBackgroundTaskInstance /* taskInstance */, _In_ BackgroundTaskCancellationReason /* cancelReason */)
    {
        IsCanceled = true;
    }
}
