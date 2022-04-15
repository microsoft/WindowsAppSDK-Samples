// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MicaWindow.h"
#include "Utilities.h"

namespace winrt
{
    using namespace Windows::UI::Composition;
}

int __stdcall WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE,  _In_ PSTR, _In_ int)
{
    // Initialize WinRt Instance
    winrt::init_apartment();

    // Enable referencing the WindowsAppSDK from an unpackaged app.
    // Remember to have a matching Microsoft.WindowsAppRuntime.Redist installed.
    // https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/deploy-unpackaged-apps
    // There are two options to initialize the bootstrapper, use the utility function here
    //      Utilities::WindowsAppSDKBootstrapperContext sdkContext;
    // or add the following tags to your .vcx project file, as done in this sample:
    //      <WindowsPackageType>None</WindowsPackageType>
    //      <LogSDKReferenceResolutionErrorsAsWarnings>true< / LogSDKReferenceResolutionErrorsAsWarnings>
    // 

    // Register Window Class before making the window
    MicaWindow::RegisterWindowClass();

    // Mica requires a compositor, which also requires a dispatcher queue
    auto controller = Utilities::CreateDispatcherQueueControllerForCurrentThread();

    auto compositor = winrt::Compositor();

    // Mica windows is inherited from the Mica Window class, which is an extension of the DesktopWindow Class.
    // Here we initialize the main window and set the title
    auto window = MicaWindow(compositor, L"Hello, Mica!");

    // Message pump.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
