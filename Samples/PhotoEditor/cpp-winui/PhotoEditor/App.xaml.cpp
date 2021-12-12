// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Navigation;
    using namespace PhotoEditor;
    using namespace PhotoEditor::implementation;
    using namespace Windows::Foundation;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

winrt::Microsoft::UI::Xaml::Window winrt::App::window{ nullptr };

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
winrt::App::App()
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

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void winrt::App::OnLaunched(LaunchActivatedEventArgs const&)
{
    window = make<MainWindow>();

    Frame rootFrame = CreateRootFrame();
    if (!rootFrame.Content())
    {
        rootFrame.Navigate(xaml_typename<PhotoEditor::MainPage>());
    }

    window.Activate();
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame that failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void winrt::App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

winrt::Frame winrt::App::CreateRootFrame()
{
    Frame rootFrame{ nullptr };
    auto content = window.Content();
    if (content)
    {
        rootFrame = content.try_as<Frame>();
    }

    // Don't repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (!rootFrame)
    {
        // Create a Frame to act as the navigation context
        rootFrame = Frame();

        rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

        // Place the frame in the current Window
        window.Content(rootFrame);
    }

    return rootFrame;
}

