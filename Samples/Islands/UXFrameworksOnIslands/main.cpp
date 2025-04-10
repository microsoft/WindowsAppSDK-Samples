// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"

#ifdef BUILD_WINDOWS
#include "ActivationContext.h"
#endif 
#include "NetUIFrame.h"
#include "Output.h"
#include "PopupFrame.h"
#include "ReactNativeFrame.h"
#include "RootFrame.h"
#include "TopLevelWindow.h"
#include "WebViewFrame.h"
#include "WinUIFrame.h"

// The DispatcherQueue.RunEventLoop() calls the ContentPreTranslateMessage API, which would allow
// Lifted Islands to receive PreTranslateMessage callbacks through InputPreTranslateKeyboardSource,
// but it would not call this app's custom System PreTranslateMessage. Thus, this app uses a
// custom event loop instead of DispatcherQueue.RunEventLoop().
void RunEventLoop(RootFrame& rootFrame) {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (ContentPreTranslateMessage(&msg))
        {
            continue;
        }

        if (rootFrame.SystemPreTranslateMessage(msg.message, msg.wParam, msg.lParam))
        {
            continue;
        }

        // Disable default behavior where Alt focuses title bar
        if (msg.message == WM_SYSKEYDOWN && msg.wParam == VK_MENU)
        {
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void InitializeFramesAndRun(
    const winrt::Compositor& compositor,
    const winrt::WUC::Compositor& systemCompositor,
    const winrt::DispatcherQueueController& dispatcherQueueController)
{
    // The structure that we are setting up below is:
    //
    //                  RootFrame
    //                 /       |
    //           NetUIFrame   WebViewFrame
    //              / 
    //        ReactNativeFrame
    //           /
    //     WinUIFrame

    // Create the top level window (which wraps an app window)
    auto queue = compositor.DispatcherQueue();

    auto settings = std::make_shared<SettingCollection>();

    TopLevelWindow topLevelWindow{ queue };

    RootFrame rootFrame{ queue, systemCompositor, settings };

    NetUIFrame netUIFrame{ queue, systemCompositor, settings };

    WebViewFrame webViewFrame{ queue, systemCompositor, settings };

    ReactNativeFrame reactNativeFrame{ compositor, settings };

    WinUIFrame winUIFrame{ compositor, settings };

    // Connect everything together. Set FrameHosts for System frames to ensure automation is 
    // being handled by the IFrameHost API as the Content automation APIs are disabled. 
    // Lifted islands are hooked up into the larger system-wide automation tree via the Content 
    // automation APIs in FragmentBased mode. See the AutomationOption setup in 
    // SystemFrame::ConnectChildFrame and LiftedFrame::ConnectChildFrame.
    topLevelWindow.ConnectFrameToWindow(&rootFrame);
    rootFrame.SetFrameHost(&topLevelWindow);
    rootFrame.SetFocusHost(&topLevelWindow);

    rootFrame.ConnectLeftFrame(&netUIFrame);
    netUIFrame.SetFrameHost(&rootFrame);

    rootFrame.ConnectRightFrame(&webViewFrame);
    webViewFrame.SetFrameHost(&rootFrame);

    netUIFrame.ConnectFrame(&reactNativeFrame);

    reactNativeFrame.ConnectLeftFrame(&winUIFrame);

    // Connect the popup frames

    PopupFrame popupRoot{ compositor, settings };

    PopupFrame popupChild{ compositor, settings };

    winUIFrame.ConnectPopupFrame(&popupRoot);

    popupRoot.ConnectPopupFrame(&popupChild);

    // Run the message loop
    RunEventLoop(rootFrame);

    dispatcherQueueController.ShutdownQueue();
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    // Initialize WinRT
    winrt::init_apartment(winrt::apartment_type::single_threaded);

#ifdef BUILD_WINDOWS
    // Set up the activation context for WinRT APIs 
    ActivationContext activationContext{};
#endif 

    // Set up the dispatcher queues
    auto dispatcherQueueController{ winrt::DispatcherQueueController::CreateOnCurrentThread() };

    // NOTE: This system DispatcherQueue is explicitly creating a dedicated thread, which is a
    // different thread than the lifted DispatcherQueue's.  It's testing a cross-thread scenario.
    auto systemDispatcherQueueController{ winrt::WS::DispatcherQueueController::CreateOnDedicatedThread() };

    // Use the current thread for the Lifted compositor 
    winrt::Compositor compositor{};

    // Use a dedicated thread for the System compositor
    winrt::WUC::Compositor systemCompositor{ nullptr };
    wil::unique_event systemCompositorCreated{ wil::EventOptions::ManualReset };
    winrt::check_bool(systemDispatcherQueueController.DispatcherQueue().TryEnqueue([&]()
    {
        systemCompositor = winrt::WUC::Compositor{};
        systemCompositorCreated.SetEvent();
    }));
    systemCompositorCreated.wait();

    InitializeFramesAndRun(compositor, systemCompositor, dispatcherQueueController);

    systemDispatcherQueueController.ShutdownQueueAsync();

    return 0;
}
