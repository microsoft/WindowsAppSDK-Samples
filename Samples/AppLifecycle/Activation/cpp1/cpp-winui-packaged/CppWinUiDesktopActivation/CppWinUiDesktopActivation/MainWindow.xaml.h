// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "MainWindow.g.h"

namespace winrt::CppWinUiDesktopActivation::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        Windows::Foundation::Collections::IVector<IInspectable> messages;

        void GetActivationInfo();

        void OutputMessage(const WCHAR* message);
        void OutputFormattedMessage(const WCHAR* fmt, ...);
        std::vector<std::wstring> SplitStrings(hstring argString);

        void ActivationInfoButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender, 
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::CppWinUiDesktopActivation::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
