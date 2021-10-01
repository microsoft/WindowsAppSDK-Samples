// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Navigation;
using namespace CppWinUiDesktopInstancing;
using namespace CppWinUiDesktopInstancing::implementation;

using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace Windows::Storage;
using namespace winrt::Microsoft::Windows;

// Standard implementation ////////////////////////////////////////////////////

App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
{
    window = make<MainWindow>();
    window.Activate();
}

///////////////////////////////////////////////////////////////////////////////


// Helpers ////////////////////////////////////////////////////////////////////

int activationCount = 1;
event_token activationToken;
Windows::Foundation::Collections::IVector<IInspectable> messages;

void OutputMessage(const WCHAR* message)
{
    messages.Append(PropertyValue::CreateString(message));
}

void OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

std::vector<std::wstring> split_strings(hstring argString)
{
    std::vector<std::wstring> argStrings;
    std::wistringstream iss(argString.c_str());
    for (std::wstring s; iss >> s; )
    {
        argStrings.push_back(s);
    }
    return argStrings;
}

///////////////////////////////////////////////////////////////////////////////


// Rich activation ////////////////////////////////////////////////////////////

void GetActivationInfo()
{
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        auto launchArgs = args.Data().as<
            Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs>();
        if (launchArgs != NULL)
        {
            auto argString = launchArgs.Arguments().c_str();
            std::vector<std::wstring> argStrings = split_strings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        auto fileArgs = args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs != NULL)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation for '%s'", file.Name().c_str());
        }
    }
}

void ReportLaunchArgs(hstring callLocation, AppActivationArguments args)
{
    Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs launchArgs = 
        args.Data().as<Windows::ApplicationModel::Activation::ILaunchActivatedEventArgs>();
    if (launchArgs != NULL)
    {
        winrt::hstring argString = launchArgs.Arguments().c_str();
        std::vector<std::wstring> argStrings = split_strings(argString);
        OutputFormattedMessage(L"Launch activation (%s)", callLocation.c_str());
        for (std::wstring s : argStrings)
        {
            OutputMessage(s.c_str());
        }
    }
}

void ReportFileArgs(hstring callLocation, AppActivationArguments args)
{
    IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
    if (fileArgs != NULL)
    {
        IStorageItem file = fileArgs.Files().GetAt(0);
        OutputFormattedMessage(
            L"File activation (%s) for '%s'", callLocation.c_str(), file.Name().c_str());
    }
}

void OnActivated(const IInspectable&, const AppActivationArguments& args)
{
    int const arraysize = 4096;
    WCHAR szTmp[arraysize];
    size_t cbTmp = arraysize * sizeof(WCHAR);
    StringCbPrintf(szTmp, cbTmp, TEXT("OnActivated, count=%d"), activationCount++);
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(szTmp, args);
    }
}

// wil requires the Microsoft.Windows.ImplementationLibrary nuget.
// https://github.com/Microsoft/wil

wil::unique_event redirectEventHandle;

winrt::fire_and_forget Redirect(AppInstance keyInstance, AppActivationArguments args)
{
    // Using this type of event ensures that it gets signaled when it 
    // goes out of scope, even if the RedirectActivationToAsync fails.
    wil::event_set_scope_exit ensure_signaled = 
        wil::SetEvent_scope_exit(redirectEventHandle.get());
    co_await keyInstance.RedirectActivationToAsync(args);
}

///////////////////////////////////////////////////////////////////////////////


// WinMain ////////////////////////////////////////////////////////////////////

// Replaces the WinMain generated in App.xaml.g.hpp, which is suppressed
// by defining DISABLE_XAML_GENERATED_MAIN in the project properties.
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    messages = winrt::single_threaded_observable_vector<IInspectable>();
    bool isRedirect = false;

    // Find out what kind of activation this is.
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        // This is a launch activation.
        ReportLaunchArgs(L"main", args);
    }
    else if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(L"main", args);

        try
        {
            // This is a file activation: here we'll get the file information,
            // and register the file name as our instance key.
            IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
            if (fileArgs != NULL)
            {
                IStorageItem file = fileArgs.Files().GetAt(0);
                AppInstance keyInstance = AppInstance::FindOrRegisterForKey(file.Name());
                OutputFormattedMessage(L"Registered key = %s", keyInstance.Key().c_str());

                // If we successfully registered the file name, we must be the
                // only instance running that was activated for this file.
                if (keyInstance.IsCurrent())
                {
                    // Report successful file name key registration.
                    OutputFormattedMessage(
                        L"IsCurrent=true; registered this instance for %s", file.Name().c_str());

                    activationToken = keyInstance.Activated([&keyInstance](
                        const auto& sender, const AppActivationArguments& args)
                        { OnActivated(sender, args); }
                    );
                    OutputMessage(L"Connected Activated event");
                }
                else
                {
                    // We're in an STA so we must not block the thread by
                    // waiting on the async call. Instead, we'll move the call
                    // to a separate thread, and use an event to synchronize.
                    isRedirect = true;
                    redirectEventHandle.create();
                    Redirect(keyInstance, args);
                    DWORD handleIndex = 0;
                    HANDLE rawHandle = redirectEventHandle.get();
                    CoWaitForMultipleObjects(CWMO_DEFAULT, INFINITE, 1, &rawHandle, &handleIndex);
                }
            }
        }
        catch (...)
        {
            OutputDebugString(L"Error getting instance information");
        }
    }

    if (!isRedirect)
    {
        ::winrt::Microsoft::UI::Xaml::Application::Start(
            [](auto&&)
            {
                ::winrt::make<::winrt::CppWinUiDesktopInstancing::implementation::App>();
            });
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
