// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "App.xaml.g.h"
#include "RegisterForCOM.h"

namespace winrt::BackgroundTaskBuilder::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        static winrt::Microsoft::UI::Xaml::Window Window();

    private:
        static winrt::Microsoft::UI::Xaml::Window window;
        RegisterForCom comRegister;
    };
}
