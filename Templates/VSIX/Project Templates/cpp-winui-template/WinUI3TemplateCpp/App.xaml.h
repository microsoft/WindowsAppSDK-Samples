// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "App.xaml.g.h"
#include "pch.h"

namespace winrt::$safeprojectname$::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        
    private:
        Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
