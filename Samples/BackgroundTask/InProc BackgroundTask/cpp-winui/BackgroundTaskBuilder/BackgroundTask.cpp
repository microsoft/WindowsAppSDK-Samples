// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "BackgroundTask.h"
#include "RegisterForCOM.h"
#include "winrt/Windows.Data.Xml.Dom.h"
#include "winrt/Windows.UI.Notifications.h"
#include "App.xaml.h"

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
        // Get deferral to indicate not to kill the background task process as soon as the Run method returns
        m_deferral = taskInstance.GetDeferral();
        m_progress = 0;
        taskInstance.Canceled({ this, &BackgroundTask::OnCanceled });

        // Calling a method on the Window to inform that the background task is executed
        winrt::Microsoft::UI::Xaml::Window window = winrt::BackgroundTaskBuilder::implementation::App::Window();
        m_mainWindow = window.as<winrt::BackgroundTaskBuilder::IMainWindow>();

        Windows::Foundation::TimeSpan period{ std::chrono::seconds{2} };
        m_periodicTimer = Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer([this, lifetime = get_strong()](Windows::System::Threading::ThreadPoolTimer timer)
            {
                if (!m_cancelRequested && m_progress < 100)
                {
                    m_progress += 10;
                }
                else
                {
                    m_periodicTimer.Cancel();

                    // Indicate that the background task has completed.
                    m_deferral.Complete();
                    if (m_cancelRequested) m_progress = -1;
                }
                m_mainWindow.BackgroundTaskExecuted(m_progress);
            }, period);
    }

    void BackgroundTask::OnCanceled(_In_ IBackgroundTaskInstance /* taskInstance */, _In_ BackgroundTaskCancellationReason /* cancelReason */)
    {
        m_cancelRequested = true;
    }
}
