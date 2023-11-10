// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::CppWinUiDesktopInstancing::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        this->StatusListView().ItemsSource(messages);
        this->Title(winrt::hstring(L"CppWinUiDesktopInstancing"));
    }
}

void winrt::CppWinUiDesktopInstancing::implementation::MainWindow::ActivationInfoButton_Click(
    winrt::Windows::Foundation::IInspectable const& /*sender*/,
    winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
{
    GetActivationInfo();
}
