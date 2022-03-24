// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
#endif

#include <sstream>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.Storage.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario1_ToastWithAvatar::rootPage{ nullptr };

    Scenario1_ToastWithAvatar::Scenario1_ToastWithAvatar()
    {
        InitializeComponent();

        InitializeComponent();
        this->StatusListView().ItemsSource(messages);

        Scenario1_ToastWithAvatar::rootPage = MainPage::Current();
    }

    void Scenario1_ToastWithAvatar::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring xmlPayload{
            L"<toast>\
                <visual>\
                    <binding template = \"ToastGeneric\">\
                        <text>App Notifications Sample Scenario 1</text>\
                        <text>This is an example message using XML</text>\
                    </binding>\
                </visual>\
                <actions>\
                    <action\
                        content = \"Activate Toast\"\
                        arguments = \"action=activateToast&amp;contentId=351\"\
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

        //winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Unregister();
    }

    void Scenario1_ToastWithAvatar::GetActivationInfo_Click(IInspectable const&, RoutedEventArgs const&)
    {
        GetActivationInfo();
    }
}
