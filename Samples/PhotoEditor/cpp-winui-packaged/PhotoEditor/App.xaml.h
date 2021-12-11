// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "App.xaml.g.h"

namespace winrt::PhotoEditor::implementation
{
    struct App : AppT<App>
    {
        App();
        Microsoft::UI::Xaml::Controls::Frame CreateRootFrame();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const&);

        static winrt::Microsoft::UI::Xaml::Window Window() { return window; };

    private:
        static winrt::Microsoft::UI::Xaml::Window window;
    };
}
