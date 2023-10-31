// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;


namespace winrt::CppWinUiDesktopActivation::implementation
{
    MainWindow::MainWindow()
    {
        messages = winrt::single_threaded_observable_vector<IInspectable>();
        InitializeComponent();
        this->StatusListView().ItemsSource(messages);
        this->Title(winrt::hstring(L"CppWinUiDesktopActivation"));
    }
}

void winrt::CppWinUiDesktopActivation::implementation::MainWindow::ActivationInfoButton_Click(
    winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
{
    GetActivationInfo();
}

void winrt::CppWinUiDesktopActivation::implementation::MainWindow::GetActivationInfo()
{
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        auto launchArgs = args.Data()
            .as<winrt::Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        auto fileArgs = args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::Protocol)
    {
        auto protocolArgs = args.Data().as<IProtocolActivatedEventArgs>();
        if (protocolArgs)
        {
            Uri uri = protocolArgs.Uri();
            OutputFormattedMessage(
                L"Protocol activation: %s", uri.RawUri().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::StartupTask)
    {
        auto startupArgs = args.Data().as<IStartupTaskActivatedEventArgs>();
        if (startupArgs)
        {
            OutputFormattedMessage(
                L"Startup activation: %s", startupArgs.TaskId().c_str());
        }
    }
}

void winrt::CppWinUiDesktopActivation::implementation::MainWindow::OutputMessage(const WCHAR* message)
{
    messages.Append(PropertyValue::CreateString(message));
}

void winrt::CppWinUiDesktopActivation::implementation::MainWindow::OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

std::vector<std::wstring> winrt::CppWinUiDesktopActivation::implementation::MainWindow::SplitStrings(hstring argString)
{
    std::vector<std::wstring> argStrings;
    std::wistringstream iss(argString.c_str());
    for (std::wstring s; iss >> s; )
    {
        argStrings.push_back(s);
    }
    return argStrings;
}
