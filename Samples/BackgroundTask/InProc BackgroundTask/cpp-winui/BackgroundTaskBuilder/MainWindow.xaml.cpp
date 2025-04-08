﻿// Copyright (c) Microsoft Corporation and Contributors.
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
    MainWindow::MainWindow()
    {
        dispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    }

    winrt::fire_and_forget MainWindow::BackgroundTaskExecuted(int progress)
    {
        co_await wil::resume_foreground(dispatcherQueue);
        if (progress < 100)
            registerButton().Content(box_value(L"Background Task Execution Progress = " + winrt::to_hstring(progress)));
        else if (progress == 100)
            registerButton().Content(box_value(L"Background Task Executed Successfully!"));
        else
            registerButton().Content(box_value(L"Background Task was cancelled!"));
    }

    void MainWindow::unregisterTasks()
    {
        // Clear up all existing background task registrations
        auto allRegistrations = BackgroundTaskRegistration::AllTasks();
        for (const auto& taskPair : allRegistrations)
        {
            IBackgroundTaskRegistration task = taskPair.Value();
            task.Unregister(true);
        }
    }

    void MainWindow::createNotification()
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

    void MainWindow::registerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        unregisterTasks();
        createNotification();

        //Register Background Task to be executed on TimeZoneChange
        winrt::Microsoft::Windows::ApplicationModel::Background::BackgroundTaskBuilder builder;

        SystemTrigger trigger = SystemTrigger(SystemTriggerType::TimeZoneChange, false);
        auto backgroundTrigger = trigger.as<IBackgroundTrigger>();
        builder.SetTrigger(backgroundTrigger);
        builder.SetTaskEntryPointClsid(__uuidof(winrt::BackgroundTaskBuilder::BackgroundTask));
        builder.Register();

        //Update Button UI
        registerButton().Content(box_value(L"Background Task Registered"));
        registerButton().IsEnabled(false);
    }
}
