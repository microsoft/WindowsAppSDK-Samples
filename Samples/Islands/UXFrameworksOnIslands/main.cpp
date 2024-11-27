// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"

#ifdef BUILD_WINDOWS
#include "ActivationContext.h"
#endif 
#include "NetUIFrame.h"
#include "Output.h"
#include "ReactNativeFrame.h"
#include "RootFrame.h"
#include "TopLevelWindow.h"
#include "WebViewFrame.h"
#include "WinUIFrame.h"

void InitializeFramesAndRun(
    const winrt::Compositor& compositor,
    const winrt::WUC::Compositor& systemCompositor,
    const winrt::DispatcherQueueController& dispatcherQueueController)
{
    // The structure that we are setting up below is:
    //
    //                  RootFrame
    //                 /       |
    //           NetUIFrameA  WebViewFrame
    //              / 
    //        ReactNativeFrame
    //           /          |
    //     NetUIFrameB  WinUIFrame

    // Create the top level window (which wraps an app window)
    TopLevelWindow topLevelWindow{ compositor };

    RootFrame rootFrame{ compositor, systemCompositor };

    NetUIFrame netUIFrameA{ compositor, systemCompositor };

    WebViewFrame webViewFrame{ compositor, systemCompositor };

    ReactNativeFrame reactNativeFrame{ compositor };

    NetUIFrame netUIFrameB{ compositor, systemCompositor };

    WinUIFrame winUIFrame{ compositor };

    // Connect everything together. Set FrameHosts for System frames to ensure automation is 
    // being handled by the IFrameHost API as the Content automation APIs are disabled. 
    // Lifted islands are hooked up into the larger system-wide automation tree via the Content 
    // automation APIs in NavigatableFragment mode. See the AutomationOption setup in 
    // SystemFrame::ConnectChildFrame and LiftedFrame::ConnectChildFrame.
    topLevelWindow.ConnectFrameToWindow(&rootFrame);
    rootFrame.SetFrameHost(&topLevelWindow);

    rootFrame.ConnectLeftFrame(&netUIFrameA);
    netUIFrameA.SetFrameHost(&rootFrame);

    rootFrame.ConnectRightFrame(&webViewFrame);
    webViewFrame.SetFrameHost(&rootFrame);

    netUIFrameA.ConnectFrame(&reactNativeFrame);

    reactNativeFrame.ConnectLeftFrame(&netUIFrameB);
    netUIFrameB.SetFrameHost(&reactNativeFrame);

    reactNativeFrame.ConnectRightFrame(&winUIFrame);

    // Run the message loop
    dispatcherQueueController.DispatcherQueue().RunEventLoop();
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
