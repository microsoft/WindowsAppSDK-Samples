// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "BackgroundTask.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;
using namespace Windows::ApplicationModel::Background;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::BackgroundTaskBuilder::implementation
{
    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::unregisterTasks()
    {
        auto allRegistrations = BackgroundTaskRegistration::AllTasks();
        for (auto taskPair : allRegistrations)
        {
            IBackgroundTaskRegistration task = taskPair.Value();
            task.Unregister(true);
        }
    }

    void MainWindow::registerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        unregisterTasks();
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

        // Using the WinAppSDK BackgroundTaskBuilder API to register a background task
        winrt::Microsoft::Windows::ApplicationModel::Background::BackgroundTaskBuilder builder;

        SystemTrigger trigger = SystemTrigger(SystemTriggerType::TimeZoneChange, false);
        auto backgroundTrigger = trigger.as<IBackgroundTrigger>();
        builder.SetTrigger(backgroundTrigger);
        builder.SetTaskEntryPointClsid(__uuidof(winrt::BackgroundTaskBuilder::BackgroundTask));
        builder.Register();
        registerButton().Content(box_value(L"Background Task Registered"));
        registerButton().IsEnabled(false);
    }
}
