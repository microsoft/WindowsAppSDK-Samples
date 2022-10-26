// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PushToastWithAvatar.h"
#include "Common.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>
#include <winrt/Windows.Foundation.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

namespace winrt
{
    using namespace Microsoft::Windows::AppNotifications;
    using namespace Microsoft::Windows::AppNotifications::Builder;
    using namespace Microsoft::Windows::PushNotifications;
    using namespace Windows::Foundation;
    using namespace CppUnpackagedAppNotifications::implementation;
}

const wchar_t* PushToastWithAvatar::ScenarioName{ L"Push / Local Toast with Avatar Image" };

// To obtain your Azure AppId, follow "Configure your app's identity in Azure Active Directory
// at https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/notifications/push/push-quickstart
const winrt::guid PushToastWithAvatar::RemoteId{ "00000000-0000-0000-0000-000000000000" }; // Replace this with your own Azure AppId

bool PushToastWithAvatar::SendToast()
{
    auto appNotification{ winrt::AppNotificationBuilder()
        .AddArgument(L"action", L"ToastClick")
        .AddArgument(Common::scenarioTag, std::to_wstring(PushToastWithAvatar::ScenarioId))
        .SetAppLogoOverride(winrt::Windows::Foundation::Uri(L"file://" + winrt::App::GetFullPathToAsset(L"Square150x150Logo.png")), winrt::AppNotificationImageCrop::Circle)
        .AddText(ScenarioName)
        .AddText(L"This is an example message using XML")
        .AddButton(winrt::AppNotificationButton(L"Open App")
            .AddArgument(L"action", L"OpenApp")
            .AddArgument(Common::scenarioTag, std::to_wstring(PushToastWithAvatar::ScenarioId)))
        .BuildNotification() };

    winrt::AppNotificationManager::Default().Show(appNotification);

    return appNotification.Id() != 0; // return true (indicating success) if the toast was sent (if it has an Id)
}

void PushToastWithAvatar::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    winrt::CppUnpackagedAppNotifications::Notification notification{};
    notification.Originator = ScenarioName;
    notification.Action = notificationActivatedEventArgs.Arguments().Lookup(L"action");
    winrt::MainPage::Current().NotificationReceived(notification);
    winrt::App::ToForeground();
}

winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel PushToastWithAvatar::RequestChannel()
{
    auto task{ RequestChannelAsync() };
    if (task.wait_for(std::chrono::minutes(5)) != winrt::AsyncStatus::Completed)
    {
        task.Cancel();
        return nullptr;
    }

    auto result{ task.GetResults() };
    return result;
}

winrt::Windows::Foundation::IAsyncOperation<winrt::PushNotificationChannel> PushToastWithAvatar::RequestChannelAsync()
{
    auto channelOperation{ winrt::PushNotificationManager::Default().CreateChannelAsync(RemoteId) };

    // Setup the in-progress event handler
    channelOperation.Progress(
        [](auto&& /*sender*/, auto&& args)
        {
            if (args.status == winrt::PushNotificationChannelStatus::InProgress)
            {
                // This is basically a noop since it isn't really an error state
                //std::cout << "\nWNS Channel URI request is in progress." << std::endl;
            }
            else if (args.status == winrt::PushNotificationChannelStatus::InProgressRetry)
            {
                //LOG_HR_MSG(
                //    args.extendedError,
                //    "The WNS Channel URI request is in back-off retry mode because of a retryable error! Expect delays in acquiring it. RetryCount = %d",
                //    args.retryCount);
            }
        });

    auto result{ co_await channelOperation };

    if (result.Status() == winrt::PushNotificationChannelStatus::CompletedSuccess)
    {
        auto channel{ result.Channel() };

        //std::cout << "\nWNS Channel URI: " << winrt::to_string(channel.Uri().ToString()) << std::endl;

        // It's the caller's responsibility to keep the channel alive
        co_return channel;
    }
    else if (result.Status() == winrt::PushNotificationChannelStatus::CompletedFailure)
    {
        //LOG_HR_MSG(result.ExtendedError(), "We hit a critical non-retryable error with the WNS Channel URI request!");
        co_return nullptr;
    }
    else
    {
        //LOG_HR_MSG(result.ExtendedError(), "Some other failure occurred.");
        co_return nullptr;
    }

};
