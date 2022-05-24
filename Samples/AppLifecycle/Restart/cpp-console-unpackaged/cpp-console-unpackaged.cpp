// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include <iostream>
#include <string>

#include <wil/result.h>
#include <wil/cppwinrt.h>

#include <winrt/Windows.ApplicationModel.Background.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>


using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation;

void printArguments()
{
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    switch (args.Kind())
    {
        case ExtendedActivationKind::Launch:
        {
            ILaunchActivatedEventArgs launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
            if (launchArgs)
            {
                winrt::hstring argString = launchArgs.Arguments();
                std::cout << "Command line launch arguments:\n" << winrt::to_string(argString) << std::endl;
            }
        }
    }
}

void restartApp()
{
    std::string restartArgs;

    std::cout << "Enter restart arguments: ";
    std::cin.ignore();
    getline(std::cin, restartArgs);

    AppRestartFailureReason restartError{ AppInstance::Restart(winrt::to_hstring(restartArgs)) };

    switch (restartError)
    {
        case AppRestartFailureReason::RestartPending:
            std::cout << "Another restart is currently pending.";
            break;
        case AppRestartFailureReason::InvalidUser:
            std::cout << "Current user is not signed in or not a valid user.";
            break;
        case AppRestartFailureReason::Other:
            std::cout << "Failure restarting.";
            break;
    }
}

int main()
{
    while (1)
    {
        int opt;
        std::cout << "Press 1: See command line launch arguments" << std::endl;
        std::cout << "Press 2: Restart this app with arguments" << std::endl;
        std::cout << "Press 0 at any time to exit App." << std::endl;

        std::cin >> opt;
        std::cout << "\n" << std::endl;

        switch (opt)
        {
            case 1:
                printArguments();
                break;
            case 2:
                restartApp();
                break;
            case 0:
                exit(1);
            default:
                break;
        }
    } 
}
