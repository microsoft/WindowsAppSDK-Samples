// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "RegisterForCOM.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace BackgroundTaskBuilder;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::BackgroundTaskBuilder::implementation
{
    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="e">Details about the launch request and process.</param>
    void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();
        window.Activate();
    }
}


int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int)
{
    if (std::wcsncmp(lpCmdLine, RegisterForCom::RegisterForComToken, sizeof(RegisterForCom::RegisterForComToken)) == 0)
    {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        RegisterForCom comRegister;
        // Start COM server and wait for the COM calls to complete
        comRegister.RegisterAndWait(__uuidof(BackgroundTask));
        OutputDebugString(L"COM Server Shutting Down");
    }
    else
    {
        // put your fancy code somewhere here
        ::winrt::Microsoft::UI::Xaml::Application::Start(
            [](auto&&)
            {
                ::winrt::make<::winrt::BackgroundTaskBuilder::implementation::App>();
            });
    }

    return 0;
}
