// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

namespace winrt
{
    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Content;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Input;
    using namespace Microsoft::UI::Windowing;

    using namespace DrawingIslandComponents;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    auto controller{ winrt::DispatcherQueueController::CreateOnCurrentThread() };
    auto queue = controller.DispatcherQueue();

    // Associate the AppWindow's lifetime with the DispatcherQueue to automatically close on exit.
    auto window = winrt::AppWindow::Create();
    window.AssociateWithDispatcherQueue(queue);
    window.Closing(
        [queue](auto&&, auto&&)
        {
            // Ensure the DispatcherQueue exits the event loop on close.
            queue.EnqueueEventLoopExit();
        });

    window.Title(L"Drawing C++ TestApp");
    window.Show();

#pragma region ...
    // Create a ContentSiteBridge and connect the ContentIsland to it.
    auto compositor = winrt::Compositor();
    auto siteBridge = winrt::DesktopChildSiteBridge::Create(compositor, window.Id());
    siteBridge.ResizePolicy(winrt::ContentSizePolicy::ResizeContentToParentWindow);
    siteBridge.Show();

    auto island = winrt::DrawingIsland(compositor).Island();
    siteBridge.Connect(island);

    // Move initial focus to the ContentIsland.
    auto focusNavigationHost = winrt::InputFocusNavigationHost::GetForSiteBridge(siteBridge);
    focusNavigationHost.NavigateFocus(winrt::FocusNavigationRequest::Create(
        winrt::FocusNavigationReason::Programmatic));
#pragma endregion

    queue.RunEventLoop();

    controller.ShutdownQueue();
}
