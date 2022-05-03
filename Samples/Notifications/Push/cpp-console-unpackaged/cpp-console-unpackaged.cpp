// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include <iostream>

#include <wil/result.h>
#include <wil/cppwinrt.h>

#include <winrt/Windows.ApplicationModel.Background.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>

using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Microsoft::Windows::PushNotifications;
using namespace winrt::Windows::Foundation;

// "To obtain your Azure AppId,
// follow "Configure your app's identity in Azure Active Directory" at https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/notifications/push/push-quickstart
winrt::guid remoteId{ "00000000-0000-0000-0000-000000000000"}; // Replace this with your own Azure AppId

winrt::Windows::Foundation::IAsyncOperation<PushNotificationChannel> RequestChannelAsync()
{
    auto channelOperation{ PushNotificationManager::Default().CreateChannelAsync(remoteId) };

    // Setup the in-progress event handler
    channelOperation.Progress(
        [](auto&& sender, auto&& args)
        {
            if (args.status == PushNotificationChannelStatus::InProgress)
            {
                // This is basically a noop since it isn't really an error state
                std::cout << "\nWNS Channel URI request is in progress." << std::endl;
            }
            else if (args.status == PushNotificationChannelStatus::InProgressRetry)
            {
                LOG_HR_MSG(
                    args.extendedError,
                    "The WNS Channel URI request is in back-off retry mode because of a retryable error! Expect delays in acquiring it. RetryCount = %d",
                    args.retryCount);
            }
        });

    auto result{ co_await channelOperation };

    if (result.Status() == PushNotificationChannelStatus::CompletedSuccess)
    {
        auto channel{ result.Channel() };

        std::cout << "\nWNS Channel URI: " << winrt::to_string(channel.Uri().ToString()) << std::endl;

        // It's the caller's responsibility to keep the channel alive
        co_return channel;
    }
    else if (result.Status() == PushNotificationChannelStatus::CompletedFailure)
    {
        LOG_HR_MSG(result.ExtendedError(), "We hit a critical non-retryable error with the WNS Channel URI request!");
        co_return nullptr;
    }
    else
    {
        LOG_HR_MSG(result.ExtendedError(), "Some other failure occurred.");
        co_return nullptr;
    }

};

winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel RequestChannel()
{
    auto task{ RequestChannelAsync() };
    if (task.wait_for(std::chrono::minutes(5)) != AsyncStatus::Completed)
    {
        task.Cancel();
        return nullptr;
    }

    auto result{ task.GetResults() };
    return result;
}

// Subscribe to an event which will get signaled whenever a foreground notification arrives.
void SubscribeForegroundEventHandler()
{
    winrt::event_token token{ PushNotificationManager::Default().PushReceived([](auto const&, PushNotificationReceivedEventArgs const& args)
    {
        auto payload{ args.Payload() };

        std::string payloadString(payload.begin(), payload.end());
        std::cout << "\nPush notification content received in the FOREGROUND: " << payloadString << std::endl;
    }) };
}

int main()
{
    auto pushNotificationManager{ PushNotificationManager::Default() };

    if (pushNotificationManager.IsSupported())
    {
        // Setup an event handler, so we can receive notifications in the foreground while the app is running.
        SubscribeForegroundEventHandler();

        pushNotificationManager.Register();
    }
    else
    {
        // Here, the app should handle the case where push notifications are not supported, for example:
        // - maintain its own persistent connection with an App Service or
        // - use polling over a scheduled interval to synchronize the client.
        std::cout << "\nPush Notifications aren't supported." << std::endl;
    }

    auto args{ AppInstance::GetCurrent().GetActivatedEventArgs() };
    switch (args.Kind())
    {
        // When it is launched normally (by the users, or from the debugger), the sample requests a WNS Channel URI and
        // displays it, then waits for notifications. This user can take a copy of the WNS Channel URI and use it to send
        // notifications to the sample
        case ExtendedActivationKind::Launch:
        {
            if (pushNotificationManager.IsSupported())
            {
                // Request a WNS Channel URI which can be passed off to an external app to send notifications to.
                // The WNS Channel URI uniquely identifies this app for this user and device.
                PushNotificationChannel channel{ RequestChannel() };
                if (!channel)
                {
                    std::cout << "\nThere was an error obtaining the WNS Channel URI" << std::endl;

                    if (remoteId == winrt::guid{ "00000000-0000-0000-0000-000000000000" })
                    {
                        std::cout << "\nThe remoteId has not been set. Refer to the readme file accompanying this sample\nfor the instructions on how to obtain and setup a remote id" << std::endl;
                    }
                }
            }
            else
            {
                // Here, the app should handle the case where push notifications are not supported, for example:
                // - maintain its own persistent connection with an App Service or
                // - use polling over a scheduled interval to synchronize the client.
                std::cout << "\nPush Notifications aren't supported." << std::endl;
            }

            std::cout << "\nPress 'Enter' at any time to exit App." << std::endl;
            std::cin.ignore();
        }
        break;

        // When it is activated from a push notification, the sample only displays the notification.
        // It doesn’t register for foreground activation of perform any other actions
        // because background activation is meant to let app perform only small tasks in order to preserve battery life.
        case ExtendedActivationKind::Push:
        {
            PushNotificationReceivedEventArgs pushArgs{ args.Data().as<PushNotificationReceivedEventArgs>() };

            // Call GetDeferral to ensure that code runs in low power
            auto deferral{ pushArgs.GetDeferral() };

            auto payload{ pushArgs.Payload() } ;

            // Do stuff to process the raw notification payload
            std::string payloadString(payload.begin(), payload.end());
            std::cout << "\nPush notification content received in the BACKGROUND: " << payloadString.c_str() << std::endl;
            std::cout << "\nPress 'Enter' to exit the App." << std::endl;

            // Call Complete on the deferral when finished processing the payload.
            // This removes the override that kept the app running even when the system was in a low power mode.
            deferral.Complete();
            std::cin.ignore();
        }
        break;

        default:
            std::cout << "\nUnexpected activation type" << std::endl;
            std::cout << "\nPress 'Enter' to exit the App." << std::endl;
            std::cin.ignore();
            break;
    }

    // We do not call PushNotificationManager::UnregisterActivator
    // because then we wouldn't be able to receive background activations, once the app has closed.
    // Call UnregisterActivator once you don't want to receive push notifications anymore.
}
