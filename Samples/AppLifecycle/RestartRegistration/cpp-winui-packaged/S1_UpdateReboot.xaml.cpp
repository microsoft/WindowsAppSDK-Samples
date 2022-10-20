// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "S1_UpdateReboot.xaml.h"
#if __has_include("S1_UpdateReboot.g.cpp")
#include "S1_UpdateReboot.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::cpp_winui_packaged::implementation
{
    S1_UpdateReboot::S1_UpdateReboot()
    {
        InitializeComponent();
        SetRecoveredMassage();
    }

    void S1_UpdateReboot::SetRecoveredMassage()
    {
        winrt::hstring cmdLineString = GetCommandLineW();
        int numArgs;
        const auto cmdLineArgs = CommandLineToArgvW(cmdLineString.c_str(), &numArgs);
        if (numArgs > 1)
        {
            recoveredMessage().Text(cmdLineArgs[1]);
            recoveryArea().Visibility(Visibility::Visible);
        }
    }

    void S1_UpdateReboot::Message_TextChanged(IInspectable const&, TextChangedEventArgs const&)
    {
        // Update registration with new arguments (only if already registered)
        DWORD flags;
        DWORD size = 0;
        if (SUCCEEDED(GetApplicationRestartSettings(GetCurrentProcess(), nullptr, &size, &flags)))
        {
            if (SUCCEEDED(RegisterApplicationRestart(messageInput().Text().c_str(), flags)))
            {
                MainPage::Current().NotifyUser(L"Updated registration", InfoBarSeverity::Informational);
            }
        }
    }
    
    void S1_UpdateReboot::Register_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Note that even after successful registration, the application will only recover if running for more than 60 seconds.
        if (SUCCEEDED(RegisterApplicationRestart(messageInput().Text().c_str(), RESTART_NO_CRASH | RESTART_NO_HANG)))
        {
            unregisterButton().IsEnabled(true);
            MainPage::Current().NotifyUser(L"Registered for restart", InfoBarSeverity::Success);
        }
        else
        {
            MainPage::Current().NotifyUser(L"Failed to register for restart", InfoBarSeverity::Error);
        }
    }

    void S1_UpdateReboot::Unregister_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (SUCCEEDED(UnregisterApplicationRestart()))
        {
            unregisterButton().IsEnabled(false);
            MainPage::Current().NotifyUser(L"Unregistered from restart", InfoBarSeverity::Success);
        }
        else
        {
            MainPage::Current().NotifyUser(L"Failed to unregister from restart", InfoBarSeverity::Error);
        }
    }
}
