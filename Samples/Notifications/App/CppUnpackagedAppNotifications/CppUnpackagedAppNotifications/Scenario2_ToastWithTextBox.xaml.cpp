// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario2_ToastWithTextBox.xaml.h"
#if __has_include("Scenario2_ToastWithTextBox.g.cpp")
#include "Scenario2_ToastWithTextBox.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario2_ToastWithTextBox::rootPage{ nullptr };

    Scenario2_ToastWithTextBox::Scenario2_ToastWithTextBox()
    {
        InitializeComponent();
#if 0
        auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
        const auto token = notificationManager.NotificationInvoked([&](const auto&, const winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs& notificationActivatedEventArgs)
            {
                rootPage.NotifyUser(L"Toast Activation Received!", InfoBarSeverity::Informational);
                //ProcessNotificationArgs(notificationActivatedEventArgs);
                winrt::Controls::ContentDialog dialog;
                dialog.Title(box_value(L"title"));
                dialog.Content(box_value(L"content"));
                dialog.PrimaryButtonText(L"primary");
                dialog.CloseButtonText(L"close");
                dialog.XamlRoot(Content().XamlRoot() /* Assuming that you're showing from the window */);

                auto result = dialog.ShowAsync();
            });

        notificationManager.Register();
#endif
        Scenario2_ToastWithTextBox::rootPage = MainPage::Current();
    }

    Scenario2_ToastWithTextBox::~Scenario2_ToastWithTextBox()
    {
        //winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().UnregisterAll();
    }

    void Scenario2_ToastWithTextBox::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring xmlPayload{
            L"<toast>\
                <visual>\
                    <binding template=\"ToastGeneric\">\
                        <text>App Notifications Sample Scenario 1</text>\
                        <text>This is an example message using XML</text>\
                    </binding>\
                </visual>\
                <actions>\
                    <action\
                        content = \"Activate Toast\"\
                        arguments = \"action=viewDetails&amp;contentId=351\"\
                        activationType = \"foreground\" />\
                </actions>\
            </toast>" };

        auto toast{ winrt::AppNotification(xmlPayload) };
        toast.Priority(winrt::AppNotificationPriority::High);
        winrt::AppNotificationManager::Default().Show(toast);
        if (toast.Id() == 0)
        {
            rootPage.NotifyUser(L"Could not send toast", InfoBarSeverity::Error);
        }

        rootPage.NotifyUser(L"Toast sent successfully!", InfoBarSeverity::Success);
    }
}
