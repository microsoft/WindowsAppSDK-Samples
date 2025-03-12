// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MyAppWindow.h"
// #include "Utilities.h"

namespace winrt
{
    using namespace Windows::UI::Composition;

    using namespace Microsoft::UI::Dispatching;
}

void RunOnIsolatedDesktop(
    std::function<void()> func)
{
    // Create the desktop and switch to it before creating any UI objects.
    HDESK hdesk = OpenDesktopW(
        L"IsolatedTestDesktop",
        0,
        FALSE,
        DESKTOP_CREATEWINDOW);

    if (hdesk != nullptr)
    {
        auto cleanup = wil::scope_exit([&]()
            {
                // Clean-up desktop
                CloseDesktop(hdesk);
            });

        if (SetThreadDesktop(hdesk))
        {
            func();
        }
    }
}

int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    RunOnIsolatedDesktop([]()
        {
            // Initialize WinRT for the thread.
            winrt::init_apartment();

            // Configure a WinAppSDK DispatcherQueue for the current thread.
            auto controller = winrt::DispatcherQueueController::CreateOnCurrentThread();
            auto queue = controller.DispatcherQueue();


            // Promote the WinAppSDK DispatcherQueue to also manage a system DispatcherQueue, which is
            // required by the system Compositor that will be used to configure Mica.
            queue.EnsureSystemDispatcherQueue();
            auto compositor = winrt::Compositor();

            // Create the app's window.
            auto window = winrt::make<winrt::MyApp::implementation::MyAppWindow>(
                queue, compositor, L"Hello, AppWindow");

            // Run the DispatcherQueue's message pump.
            queue.RunEventLoop();
        });

    return 0;
}
