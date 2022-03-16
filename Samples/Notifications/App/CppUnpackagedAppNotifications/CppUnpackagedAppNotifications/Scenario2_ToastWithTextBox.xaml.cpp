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

        Scenario2_ToastWithTextBox::rootPage = MainPage::Current();
    }

    void Scenario2_ToastWithTextBox::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring xmlPayload{
            L"<toast>\
                <visual>\
                    <binding template = \"ToastGeneric\">\
                        <text>App Notifications Sample Scenario 2</text>\
                        <text>This is an example message using XML</text>\
                    </binding>\
                </visual>\
                <actions>\
                    <input\
                        id = \"tbReply\"\
                        type = \"text\"\
                        placeHolderContent = \"Type a reply\"/>\
                    <action\
                        content = \"Reply\"\
                        arguments = \"action=reply&amp;contentId=351\"\
                        activationType = \"background\" />\
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
