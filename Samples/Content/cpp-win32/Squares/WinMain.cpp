// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DrawingIsland.h"


int __stdcall WinMain(
    _In_ HINSTANCE,
    _In_opt_ HINSTANCE,
    _In_ PSTR,
    _In_ int)
{
    // This application is referencing WindowsAppSDK using the unpackaged sceanario
    // by setting <WindowsPackageType>None</WindowsPackageType> in the project file.
    // https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/deploy-unpackaged-apps

    // Initialize current thread for COM/WinRT.
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Create a Lifted (WinAppSDK) DispatcherQueue for this thread.  This is needed for
    // Microsoft.UI.Composition, Content, and Input APIs.
    auto dispatcherQueueController =
        winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();

    auto dispatcherQueue = dispatcherQueueController.DispatcherQueue();

    // Create a top level AppWindow and show it.
    auto appWindow = winrt::AppWindow::Create();
    appWindow.Title(L"Squares");
    appWindow.Show();

    // When the AppWindow is closing, post a quit to the message loop so we return from WinMain.
    auto destroyingEventToken = appWindow.Destroying(
        [&](winrt::AppWindow const& /*sender*/,
            winrt::IInspectable const& /*args*/)
        {
            dispatcherQueue.EnqueueEventLoopExit();
        });

    // Create a Compositor for all Content on this thread.
    auto compositor = winrt::Microsoft::UI::Composition::Compositor();

    // Create a DesktopChildSiteBridge that allows Content to be hosted in a HWND environment.
    auto desktopChildSiteBridge = winrt::DesktopChildSiteBridge::Create(compositor, appWindow.Id());

    // Make the DesktopChildSiteBridge visible. 
    desktopChildSiteBridge.Show();

    // Set the resize policy so the DesktopChildSiteBridge matches its parent HWND size.
    desktopChildSiteBridge.ResizePolicy(winrt::ContentSizePolicy::ResizeContentToParentWindow);

    // Create a DrawingIsland that uses a ContentIsland to provide Output, Input,
    // Layout and Accesiblity functionality.
    winrt::com_ptr<Squares::DrawingIsland> drawingIsland =
        winrt::make_self<Squares::DrawingIsland>(compositor);

    // Connect the DesktopChildSiteBridge with the ContentIsland. 
    desktopChildSiteBridge.Connect(drawingIsland->Island());

    // Run the event loop until somebody posts a quit message.
    dispatcherQueue.RunEventLoop();

    appWindow.Destroying(destroyingEventToken);

    dispatcherQueueController.ShutdownQueue();

    return 0;
}
