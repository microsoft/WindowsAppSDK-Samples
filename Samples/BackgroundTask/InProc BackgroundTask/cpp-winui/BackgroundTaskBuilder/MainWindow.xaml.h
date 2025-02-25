// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::BackgroundTaskBuilder::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        Microsoft::UI::Dispatching::DispatcherQueue dispatcherQueue = nullptr;

        winrt::fire_and_forget BackgroundTaskExecuted();

        void registerButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void unregisterTasks();
    };
}

namespace winrt::BackgroundTaskBuilder::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
