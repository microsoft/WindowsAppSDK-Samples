﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ToastWithTextBox.h"
#include "Common.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

namespace winrt
{
    using namespace Microsoft::Windows::AppNotifications;
    using namespace CppUnpackagedAppNotifications::implementation;
}

bool ToastWithTextBox::SendToast()
{
    winrt::hstring xmlPayload{
        L"<toast>\
                <visual>\
                    <binding template = \"ToastGeneric\">\
                        <image placement = \"appLogoOverride\" hint-crop=\"circle\" src = \"" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png") + L"\"/>\
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
                        arguments = \"action=reply&amp;" + Common::MakeScenarioIdToken(ToastWithTextBox::ScenarioId) + L"\"\
                        hint-inputId=\"tbReply\"/>\
                </actions>\
            </toast>" };

    auto toast{ winrt::AppNotification(xmlPayload) };
    toast.Priority(winrt::AppNotificationPriority::High);
    winrt::AppNotificationManager::Default().Show(toast);
    if (toast.Id() == 0)
    {
        return false;
    }

    return true;
}

void ToastWithTextBox::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto input{ notificationActivatedEventArgs.UserInput() };
    auto text{ input.Lookup(L"tbReply") };

    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = L"Scenario2_ToastWithTextBox";
    auto action{ Common::ExtractParamFromArgs(notificationActivatedEventArgs.Argument().c_str(), L"action") };
    notification.Action = action.has_value() ? action.value() : L"";
    notification.HasInput = true;
    notification.Input = text;
    winrt::MainPage::Current().NotificationReceived(notification);
}
