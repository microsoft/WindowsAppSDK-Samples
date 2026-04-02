// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.
#pragma once

#include "App.xaml.g.h"

namespace winrt::TestOAuthInCPlusPlus::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        static HWND WindowHandle() noexcept;
        static winrt::Microsoft::UI::Xaml::Window Window() noexcept;

        static inline winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        static inline HWND windowHandle{ nullptr };
    };
}
