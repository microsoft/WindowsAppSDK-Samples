// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "TopLevelWindow.h"
#include "NetUIFrame.h"
#include "VisualUtils.h"

TopLevelWindow::TopLevelWindow(
    const winrt::Compositor& compositor)
{
    RegisterWindowClass();
    InitializeWindow();
    InitializeBridge(compositor);
    InitializeAutomation();
}

void TopLevelWindow::ConnectFrameToWindow(
    IFrame* frame)
{
    m_bridge.Connect(frame->GetIsland());

    m_frame = frame;
}

// AutomationHelpers::IAutomationFragmentCallbackHandler implementation
winrt::Windows::Graphics::RectInt32 TopLevelWindow::GetBoundingRectangleInScreenCoordinatesForAutomation(
    ::IUnknown const* const sender) const
{
    if (sender != m_windowAutomationRoot->GetIUnknown<AutomationHelpers::AutomationFragment>())
    {
        // Return an empty rectangle if the sender is not our window's automation root.
        return { 0, 0, 0, 0 };
    }

    // Get the window rect in screen pixels.
    RECT rect{};
    ::GetWindowRect(m_hwnd, &rect);
    winrt::Windows::Graphics::RectInt32 boundingRect{};
    boundingRect.X = rect.left;
    boundingRect.Y = rect.top;
    boundingRect.Width = rect.right - rect.left;
    boundingRect.Height = rect.bottom - rect.top;

    return boundingRect;
}


void TopLevelWindow::HandleSetFocusForAutomation(
    ::IUnknown const* const)
{
    // Our automation provider does not have any special focus handling logic.
    return;
}

// AutomationHelpers::IAutomationFragmentRootCallbackHandler implementation
winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetFragmentFromPointInScreenCoordinatesForAutomation(
    double x,
    double y,
    ::IUnknown const* const sender) const
{
    if (sender != m_windowAutomationRoot->GetIUnknown<AutomationHelpers::AutomationFragmentRoot>())
    {
        // Return nullptr if the sender is not our window's automation root.
        return nullptr;
    }

    // Get the client rect in screen pixels.
    RECT rect{};
    ::GetClientRect(m_hwnd, &rect);
    ::MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);

    if (x < rect.left || x >= rect.right || y < rect.top || y >= rect.bottom)
    {
        // The point is outside the client area.
        return nullptr;
    }

    // We have only one child fragment since we contain only one frame.
    // If the child fragment is not hit, we return ourselves.
    winrt::com_ptr<::IRawElementProviderFragment> childFragment{ nullptr };
    winrt::check_hresult(m_frame->GetAutomationProvider()->ElementProviderFromPoint(x, y, childFragment.put()));
    return (nullptr != childFragment) ? childFragment : m_windowAutomationRoot.as<::IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetFragmentInFocusForAutomation(
    ::IUnknown const* const sender) const
{
    if (sender != m_windowAutomationRoot->GetIUnknown<AutomationHelpers::AutomationFragmentRoot>())
    {
        // Return nullptr if the sender is not our window's automation root.
        return nullptr;
    }

    // We have only one child fragment since we contain only one frame.
    return m_frame->GetAutomationProvider().as<IRawElementProviderFragment>();
}

// AutomationHelpers::IAutomationExternalChildCallbackHandler implementation
winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetFirstChildFragmentForAutomation(
        ::IUnknown const* const sender) const
{
    if (sender != m_windowAutomationRoot->GetIUnknown<AutomationHelpers::AutomationFragment>())
    {
        // Return nullptr if the sender is not our window's automation root.
        return nullptr;
    }

    // We have only one child fragment since we contain only one frame.
    return m_frame->GetAutomationProvider().as<IRawElementProviderFragment>();
}

winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetLastChildFragmentForAutomation(
    ::IUnknown const* const sender) const
{
    if (sender != m_windowAutomationRoot->GetIUnknown<AutomationHelpers::AutomationFragment>())
    {
        // Return nullptr if the sender is not our window's automation root.
        return nullptr;
    }

    // We have only one child fragment since we contain only one frame.
    return m_frame->GetAutomationProvider().as<IRawElementProviderFragment>();
}

// IFrameHost implementation
winrt::com_ptr<::IRawElementProviderFragmentRoot> TopLevelWindow::GetFragmentRootProviderForChildFrame(_In_ IFrame const* const sender) const
{
    return (sender == m_frame) ? m_windowAutomationRoot : nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetNextSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // The RootFrame is the only frame directly connected to the ReadOnlyDesktopSiteBridge.
    return nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetParentProviderForChildFrame(_In_ IFrame const* const sender) const
{
    return (sender == m_frame) ? m_windowAutomationRoot : nullptr;
}

winrt::com_ptr<::IRawElementProviderFragment> TopLevelWindow::GetPreviousSiblingProviderForChildFrame(_In_ IFrame const* const) const
{
    // The RootFrame is the only frame directly connected to the ReadOnlyDesktopSiteBridge.
    return nullptr;
}

/*static*/
LRESULT CALLBACK TopLevelWindow::WindowProc(
    HWND hwnd, 
    UINT message, 
    WPARAM wparam, 
    LPARAM lparam)
{
    // Retrieve the 'this' pointer from the user data
    TopLevelWindow* pThis = reinterpret_cast<TopLevelWindow*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (message == WM_NCDESTROY)
    {
        // Clear the 'this' pointer from the user data
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
    }

    if (pThis)
    {
        return pThis->MessageHandler(message, wparam, lparam);
    }
    else
    {
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }
}

LRESULT TopLevelWindow::MessageHandler(
    UINT message, 
    WPARAM wparam, 
    LPARAM lparam)
{
    // First process messages ourselves
    bool handled = false;
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            // Take Win32 focus on mouse click to the top level window.
            // Don't mark as handled so the message can be forwarded to the frame.
            ::SetFocus(m_hwnd);
            break;
        }

        case WM_GETOBJECT:
        {
            // Handle WM_GETOBJECT to provide UI Automation support
            if (m_windowAutomationRoot != nullptr && (UiaRootObjectId == static_cast<DWORD>(lparam)))
            {
                m_windowAutomationRootRegisteredWithUIA = true;
                return ::UiaReturnRawElementProvider(m_hwnd, wparam, lparam, m_windowAutomationRoot.get());
            }
            break;
        }
    }

    // Forward the message to the frame
    if (m_frame != nullptr && !handled)
    {
        LRESULT result = m_frame->HandleMessage(message, wparam, lparam, &handled);
        if (handled)
        {
            return result;
        }
    }

    return ::DefWindowProc(m_hwnd, message, wparam, lparam);
}

void TopLevelWindow::RegisterWindowClass()
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = TopLevelWindow::WindowProc;
    wcex.hInstance = ::GetModuleHandle(nullptr);
    wcex.lpszClassName = k_windowClassName;
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

    ::RegisterClassExW(&wcex);
}

void TopLevelWindow::InitializeWindow()
{
    // Lifetime notes:
    // - The TopLevelWindow object outlives the message loop
    // - This means that 'this' is always valid in window messages,
    //   and we don't need reference counting or weak pointers.

    m_hwnd = ::CreateWindowExW(
        0,
        k_windowClassName,
        k_windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        nullptr,
        nullptr,
        ::GetModuleHandle(nullptr),
        this);

    THROW_LAST_ERROR_IF_NULL(m_hwnd);

    // Set 'this' as the user data for the window
    ::SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    auto windowId = winrt::GetWindowIdFromWindow(m_hwnd);
    m_window = winrt::AppWindow::GetFromWindowId(windowId);

    m_closingRevoker = { m_window, m_window.Closing([this](auto&&, auto&&)
    {
        // Stop the message loop
        auto dispatcherQueue = winrt::DispatcherQueue::GetForCurrentThread();
        dispatcherQueue.EnqueueEventLoopExit();

        // If we registered our automation root with UIA, we need to unregister it.
        if (m_windowAutomationRootRegisteredWithUIA)
        {
            m_fragmentRevoker.reset();
            m_fragmentRootRevoker.reset();

            winrt::check_hresult(::UiaDisconnectProvider(m_windowAutomationRoot.get()));
            ::UiaReturnRawElementProvider(m_hwnd, 0, 0, nullptr);

            m_windowAutomationRootRegisteredWithUIA = false;
        }
    }) };

    m_window.AssociateWithDispatcherQueue(winrt::DispatcherQueue::GetForCurrentThread());

    m_window.Show();
}

void TopLevelWindow::InitializeBridge(
    const winrt::Compositor& compositor)
{
    auto windowId = m_window.Id();
    m_bridge = winrt::ReadOnlyDesktopSiteBridge::Create(compositor, windowId);
    m_bridge.OverrideScale(1.0f);
}

void TopLevelWindow::InitializeAutomation()
{
    m_windowAutomationRoot = winrt::make_self<AutomationHelpers::AutomationFragmentRoot>();
    m_windowAutomationRoot->Name(k_windowAutomationName);
    m_windowAutomationRoot->UiaControlTypeId(UIA_WindowControlTypeId);
    m_windowAutomationRoot->HostProviderFromHwnd(m_hwnd);

    // Register ourselves for responding to UIA callbacks.
    m_fragmentRevoker = m_windowAutomationRoot->SetFragmentCallbackHandler(this);
    m_fragmentRootRevoker = m_windowAutomationRoot->SetFragmentRootCallbackHandler(this);
    m_externalChildRevoker = m_windowAutomationRoot->SetExternalChildCallbackHandler(this);
}
