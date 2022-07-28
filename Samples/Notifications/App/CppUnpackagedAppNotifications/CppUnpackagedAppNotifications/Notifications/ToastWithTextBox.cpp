// Copyright (c) Microsoft Corporation.
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

const wchar_t* ToastWithTextBox::ScenarioName{ L"Local Toast with Avatar and Text Box" };
auto textboxReplyId{ L"textboxReply" };

bool ToastWithTextBox::SendToast()
{
    // The ScenarioIdToken uniquely identify a scenario and is used to route the response received when the user clicks on a toast to the correct scenario.
    auto ScenarioIdToken{ Common::MakeScenarioIdToken(ToastWithTextBox::ScenarioId) };

    winrt::hstring xmlPayload{
        L"<toast launch = \"action=ToastClick&amp;" + ScenarioIdToken + L"\">\
            <visual>\
                <binding template = \"ToastGeneric\">\
                    <image placement = \"appLogoOverride\" hint-crop=\"circle\" src = \"" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png") + L"\"/>\
                    <text>" + ScenarioName + L"</text>\
                    <text>This is an example message using XML</text>\
                </binding>\
            </visual>\
                <actions>\
                    <input\
                        id = \"" + textboxReplyId + L"\"\
                        type = \"text\"\
                        placeHolderContent = \"Type a reply\"/>\
                    <action\
                        content = \"Reply\"\
                        arguments = \"action=Reply&amp;" + ScenarioIdToken + L"\"\
                        hint-inputId=\"" + textboxReplyId + L"\"/>\
                </actions>\
            </toast>" };

    auto toast{ winrt::AppNotification(xmlPayload) };
    winrt::AppNotificationManager::Default().Show(toast);

    return toast.Id() != 0; // return true (indicating success) if the toast was sent (if it has an Id)
}

void ToastWithTextBox::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    // In a real-life scenario, this type of action would usually be processed in the background. Your App would process the payload in
    // the background (sending the payload back to your App Server) without ever showing the App's UI.
    // This is not something that can easily be demonstrated in a sample such as this one, as we need to show the UI to demonstrate how
    // the payload is routed internally

    auto input{ notificationActivatedEventArgs.UserInput() };
    auto text{ input.Lookup(textboxReplyId) };

    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = ScenarioName;
    auto action{ Common::ExtractParamFromArgs(notificationActivatedEventArgs.Argument().c_str(), L"action") };
    notification.Action = action.has_value() ? action.value() : L"";
    notification.HasInput = true;
    notification.Input = text;
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}
