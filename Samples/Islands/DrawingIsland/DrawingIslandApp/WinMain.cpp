// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

namespace winrt
{
    using namespace Microsoft::UI;
    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Content;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Input;
    using namespace Microsoft::UI::Windowing;

    using namespace DrawingIslandComponents;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    auto controller{ winrt::DispatcherQueueController::CreateOnCurrentThread() };
    auto queue = controller.DispatcherQueue();

    auto window = winrt::AppWindow::Create();
    window.AssociateWithDispatcherQueue(queue);
    window.Closing([&](auto&&, auto&&)
        {
            queue.EnqueueEventLoopExit();
        });

    window.Title(L"Drawing Island");
    window.Show();

    auto compositor = winrt::Compositor();
    auto drawing = winrt::DrawingIsland(compositor);

    auto siteBridge = winrt::DesktopChildSiteBridge::Create(compositor, window.Id());
    siteBridge.ResizePolicy(winrt::ContentSizePolicy::ResizeContentToParentWindow);
    siteBridge.Show();
    siteBridge.Connect(drawing.Island());

    queue.RunEventLoop();

    controller.ShutdownQueue();
}
