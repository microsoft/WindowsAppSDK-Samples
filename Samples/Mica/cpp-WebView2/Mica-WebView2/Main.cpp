// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "CompositionWindow.h"
#include "WebView2Window.h"
#include "Utilities.h"

namespace winrt
{
    using namespace Windows::UI::Composition;
}

int __stdcall WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ int)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Enable referencing the WindowsAppSDK from an unpackaged app.
    // Remember to have a matching Microsoft.WindowsAppRuntime.Redist installed.
    // https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/deploy-unpackaged-apps
    Utilities::WindowsAppSDKBootstrapperContext sdkContext;

    CompositionWindow::RegisterWindowClass();

    // A dispatcher queue is required to be able to create a compositor.
    auto controller = Utilities::CreateDispatcherQueueControllerForCurrentThread();

    auto compositor = winrt::Compositor();

    auto window = WebView2Window(compositor, L"Hello, WebView2!");

    // Message pump.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
