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

    auto compositor = winrt::Compositor();
    auto island = winrt::DrawingIsland(compositor).Island();

    auto window = winrt::AppWindow::Create();
    window.AssociateWithDispatcherQueue(queue);
    window.Closing(
        [island, queue](auto&&, auto&&)
        {
            island.Close();
            queue.EnqueueEventLoopExit();
        });

    window.Title(L"Drawing C++ TestApp");
    window.Show();

    auto siteBridge = winrt::DesktopChildSiteBridge::Create(compositor, window.Id());
    siteBridge.ResizePolicy(winrt::ContentSizePolicy::ResizeContentToParentWindow);
    siteBridge.Show();
    siteBridge.Connect(island);

    // Move initial focus to the island.
    auto focusNavigationHost = winrt::InputFocusNavigationHost::GetForSiteBridge(siteBridge);
    focusNavigationHost.NavigateFocus(winrt::FocusNavigationRequest::Create(
        winrt::FocusNavigationReason::Programmatic));

    queue.RunEventLoop();

    controller.ShutdownQueue();
}
