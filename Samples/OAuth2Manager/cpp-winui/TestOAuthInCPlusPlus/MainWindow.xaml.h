// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.
#pragma once

#include "MainWindow.g.h"

namespace winrt::TestOAuthInCPlusPlus::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        winrt::Windows::Foundation::IAsyncAction implicitGrantWithRedirectButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::Windows::Foundation::IAsyncAction authCodeGrantWithRedirectButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void OnUriCallback(winrt::Windows::Foundation::Uri const& responseUri);

    private:
        winrt::Microsoft::UI::WindowId parentWindowId;
        Microsoft::UI::Dispatching::DispatcherQueue dispatcherQueue{nullptr};
    };
}

namespace winrt::TestOAuthInCPlusPlus::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
