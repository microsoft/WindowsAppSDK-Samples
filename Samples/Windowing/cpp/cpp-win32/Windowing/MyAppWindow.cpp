// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "MyAppWindow.h"

namespace winrt
{
    using namespace Windows::UI::Composition;

    using namespace Microsoft::UI;
    using namespace Microsoft::UI::Composition::SystemBackdrops;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Windowing;
}

namespace winrt::MyApp::implementation
{
    // Create the AppWindow and enable Mica.
    MyAppWindow::MyAppWindow(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        const winrt::Compositor& compositor,
        const std::wstring& windowTitle)
    {
        m_queue = queue;

        // Create a new window and associate its lifetime with the DispatcherQueue.
        m_appWindow = winrt::AppWindow::Create();
        m_appWindow.AssociateWithDispatcherQueue(m_queue);

        auto windowId = m_appWindow.Id();
        m_appWindow.Destroying({ get_strong(), &MyAppWindow::Window_Destroying });

        // Create a normal overlapped window with a title bar.
        m_appWindow.Title(windowTitle);
        m_appWindow.SetPresenter(winrt::AppWindowPresenterKind::Overlapped);

        // After fully configuring the window, create and show it.
        m_appWindow.Create();
        m_appWindow.Show();

        // Configure the composition target before enabling Mica for this window.
        HWND hwnd = winrt::GetWindowFromWindowId(windowId);
        auto interop = compositor.as<
            ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();

        winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{ nullptr };
        winrt::check_hresult(interop->CreateDesktopWindowTarget(
            hwnd,
            true,
            reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget**>(
                winrt::put_abi(target))));

        m_target = target;
        m_target.Root(compositor.CreateContainerVisual());

        m_micaController = winrt::MicaController();
        m_isMicaSupported = m_micaController.SetTarget(windowId, m_target);
    }

    void MyAppWindow::Window_Destroying(
        const winrt::Microsoft::UI::Windowing::AppWindow& /*sender*/,
        const winrt::Windows::Foundation::IInspectable& /*args*/)
    {
        // Exit the message loop when the window is destroyed.
        m_queue.EnqueueEventLoopExit();

        m_micaController = nullptr;
        m_appWindow = nullptr;
    }
}
