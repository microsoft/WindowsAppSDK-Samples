// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
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
    MainPage Scenario1_ToastWithAvatar::rootPage{ nullptr };

    Scenario1_ToastWithAvatar::Scenario1_ToastWithAvatar()
    {
        InitializeComponent();

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

        Scenario1_ToastWithAvatar::rootPage = MainPage::Current();
    }

    Scenario1_ToastWithAvatar::~Scenario1_ToastWithAvatar()
    {
        winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().UnregisterAll();
    }

    void Scenario1_ToastWithAvatar::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
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
