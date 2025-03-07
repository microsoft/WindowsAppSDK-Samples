// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MyAppWindow.h"
#include "Utilities.h"

namespace winrt
{
    using namespace Windows::UI::Composition;

    using namespace Microsoft::UI::Dispatching;
}

int __stdcall WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ int)
{
    // Initialize WinRT for the thread.
    winrt::init_apartment();

    // Enable referencing the WindowsAppSDK from an unpackaged app.
    // Remember to have a matching Microsoft.WindowsAppRuntime.Redist installed.
    // https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/deploy-unpackaged-apps
    // There are two options to initialize the bootstrapper, use the utility function here
    //      Utilities::WindowsAppSDKBootstrapperContext sdkContext;
    // or add the following tags to your .vcx project file, as done in this sample:
    //      <WindowsPackageType>None</WindowsPackageType>
    //      <LogSDKReferenceResolutionErrorsAsWarnings>true< / LogSDKReferenceResolutionErrorsAsWarnings>

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

    return 0;
}
