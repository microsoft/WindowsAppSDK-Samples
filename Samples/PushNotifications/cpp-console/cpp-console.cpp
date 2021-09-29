// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include <windows.h>
#include <iostream>

<<<<<<< Updated upstream
#include <unknwn.h>
#include <appmodel.h>
#include <wil/result.h>
#include <wil/cppwinrt.h>
#include <wil/resource.h>

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
=======
//#include <unknwn.h>
#include <appmodel.h>
#include <wil/result.h>
#include <wil/cppwinrt.h>
//#include <wil/resource.h>

//#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.h>
//#include <winrt/Windows.Foundation.Collections.h>
//#include <winrt/Windows.Storage.h>
//#include <winrt/Windows.Storage.Streams.h>
>>>>>>> Stashed changes
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Microsoft.Windows.PushNotifications.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>

<<<<<<< Updated upstream
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Windows.ApplicationModel.Resources.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Resources;

using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Microsoft::Windows::PushNotifications;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Background; // BackgroundTask APIs
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace Windows::Globalization::DateTimeFormatting;
=======
//#include "winrt\Windows.Foundation.h"
//#include "winrt\Windows.Foundation.Collections.h"
//#include "winrt\Windows.ApplicationModel.Resources.h"

//using namespace winrt;
//using namespace winrt::Windows::ApplicationModel::Resources;

using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Microsoft::Windows::PushNotifications;
//using namespace winrt::Windows::ApplicationModel::Activation;
//using namespace winrt::Windows::ApplicationModel::Background; // BackgroundTask APIs
using namespace winrt::Windows::Foundation;
//using namespace winrt::Windows::Storage;
//using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Globalization::DateTimeFormatting;
>>>>>>> Stashed changes

// To obtain an AAD RemoteIdentifier for your app,
// follow the instructions on https://docs.microsoft.com/azure/active-directory/develop/quickstart-register-app
//winrt::guid remoteId{ "00000000-0000-0000-0000-000000000000"}; // Replace this with own remoteId
<<<<<<< Updated upstream
winrt::guid remoteId{ "0160ee84-0c53-4851-9ff2-d7f5a87ed914" };
=======
>>>>>>> Stashed changes

winrt::Windows::Foundation::IAsyncOperation<PushNotificationChannel> RequestChannelAsync()
{
    auto channelOperation = PushNotificationManager::CreateChannelAsync(remoteId);

    // Setup the inprogress event handler
    channelOperation.Progress(
        [](auto&& sender, auto&& args)
        {
            if (args.status == PushNotificationChannelStatus::InProgress)
            {
                // This is basically a noop since it isn't really an error state
                std::cout << "Channel request is in progress." << std::endl << std::endl;
            }
            else if (args.status == PushNotificationChannelStatus::InProgressRetry)
            {
                LOG_HR_MSG(
                    args.extendedError,
                    "The channel request is in back-off retry mode because of a retryable error! Expect delays in acquiring it. RetryCount = %d",
                    args.retryCount);
            }
        });

    auto result = co_await channelOperation;

    if (result.Status() == PushNotificationChannelStatus::CompletedSuccess)
    {
        // Caller's responsibility to keep the channel alive
        co_return result.Channel();
    }
    else if (result.Status() == PushNotificationChannelStatus::CompletedFailure)
    {
        LOG_HR_MSG(result.ExtendedError(), "We hit a critical non-retryable error with channel request!");
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
    auto task = RequestChannelAsync();
    if (task.wait_for(std::chrono::seconds(300)) != AsyncStatus::Completed)
    {
        task.Cancel();
        return nullptr;
    }

    auto result = task.GetResults();
    return result;
}

int main()
{
    if (PushNotificationManager::IsActivatorSupported(PushNotificationRegistrationActivators::ComActivator))
    {
        PushNotificationActivationInfo info(
            PushNotificationRegistrationActivators::PushTrigger | PushNotificationRegistrationActivators::ComActivator,
            winrt::guid("0160ee84-0c53-4851-9ff2-d7f5a87ed914")); // same clsid as app manifest

        PushNotificationManager::RegisterActivator(info);
	}

   
    auto args = AppInstance::GetCurrent().GetActivatedEventArgs();
    auto kind = args.Kind();
    switch (kind)
    {

    case ExtendedActivationKind::Launch:
    {

        // request a channel
        PushNotificationChannel channel = RequestChannel();

        // register the chaneel
        if (channel)
        {
            auto channelUri = channel.Uri();
            DateTimeFormatter formater = DateTimeFormatter(L"on {month.abbreviated} {day.integer(1)}, {year.full} at {hour.integer(1)}:{minute.integer(2)}:{second.integer(2)}");

            std::cout << "Channel Uri: " << winrt::to_string(channelUri.ToString()) << std::endl << std::endl;
            std::wcout << L"Channel Uri will expire " << formater.Format(channel.ExpirationTime()).c_str() << std::endl;

            // Register Push Event for Foreground
            winrt::event_token token = channel.PushReceived([](const auto&, PushNotificationReceivedEventArgs const& args)
                {
                    auto payload = args.Payload();

                    // Do stuff to process the raw payload
                    std::string payloadString(payload.begin(), payload.end());
                    std::cout << "Push notification content received from FOREGROUND: " << payloadString << std::endl << std::endl;
                    args.Handled(true);
                });
        }

        std::cout << "Press 'Enter' at any time to exit App." << std::endl;
        std::cin.ignore();
    }
    break;

    case ExtendedActivationKind::Push:
    {
        PushNotificationReceivedEventArgs pushArgs = args.Data().as<PushNotificationReceivedEventArgs>();

        // Call GetDeferral to ensure that code runs in low power
        auto deferral = pushArgs.GetDeferral();

        auto payload = pushArgs.Payload();

        // Do stuff to process the raw payload
        std::string payloadString(payload.begin(), payload.end());
        std::cout << "Push notification content received from BACKGROUND: " << payloadString.c_str() << std::endl;
        std::cout << "Press 'Enter' to exit the App." << std::endl;

        // Call Complete on the deferral when finished processing the payload.
        // This removes the override that kept the app running even when the system was in a low power mode.
<<<<<<< Updated upstream
        //deferral.Complete();
=======
        deferral.Complete();
>>>>>>> Stashed changes
        std::cin.ignore();
    }
    break;

    default:
        // Unexpected activation type
        std::cout << "Unexpected activation type";
        std::cout << "Press 'Enter' to exit the App.";
        std::cin.ignore();
        break;
    } //switch

   if (PushNotificationManager::IsActivatorSupported(PushNotificationRegistrationActivators::ComActivator))
    {
        // Don't unregister PushTrigger because we still want to receive push notifications from background infrastructure.
        PushNotificationManager::UnregisterActivator(PushNotificationRegistrationActivators::ComActivator);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
