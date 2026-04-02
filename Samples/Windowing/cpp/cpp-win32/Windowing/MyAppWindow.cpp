// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "MyAppWindow.h"
using namespace winrt::Microsoft::UI::Windowing;

// global
winrt::Microsoft::UI::Windowing::AppWindow myAppWindow{ nullptr };

namespace winrt
{
    using namespace Microsoft::UI;
    using namespace Microsoft::UI::Windowing;
    using namespace Microsoft::UI::Composition::SystemBackdrops;
    using namespace Windows::UI::Composition;
}

// static
const std::wstring MyAppWindow::ClassName = L"MyAppWindow";

// static
void MyAppWindow::RegisterWindowClass()
{
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    WNDCLASSEX wcex = { sizeof(wcex) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIconW(instance, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = ClassName.c_str();
    wcex.hIconSm = LoadIconW(wcex.hInstance, IDI_APPLICATION);
    winrt::check_bool(RegisterClassExW(&wcex)); // check if the window class was registered succesfully
}

// Create the AppWindow and enable Mica
MyAppWindow::MyAppWindow(const winrt::Compositor& compositor, const std::wstring& windowTitle)
{
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    WINRT_ASSERT(!m_window); // check that window is not initialized
    WINRT_VERIFY(
        // Window Properties
        CreateWindowExW(
            WS_EX_COMPOSITED,
            ClassName.c_str(), // declared in MyAppWindow.h and defined above
            windowTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT, 
            800, 600, 
            nullptr, 
            nullptr, 
            instance, 
            this
        ));

    //Create AppWindow from existing hWnd)
    winrt::WindowId windowId{ (UINT64) m_window };
    windowId = winrt::GetWindowIdFromWindow(m_window);

    // Get the AppWindow for the WindowId
    myAppWindow = winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(windowId);

    //Create Compact Overlay Presenter
    winrt::Microsoft::UI::Windowing::AppWindowPresenterKind newPresenterKind = winrt::Microsoft::UI::Windowing::AppWindowPresenterKind::CompactOverlay;
    myAppWindow.SetPresenter(newPresenterKind);

    myAppWindow.Create();
    myAppWindow.Show();

    // The Mica controller needs to set a target with a root to recognize the visual base layer
    m_target = CreateWindowTarget(compositor);

    // Need to set a root before we can enable Mica.
    m_target.Root(compositor.CreateContainerVisual());

    m_micaController = winrt::MicaController();
    m_isMicaSupported = m_micaController.SetTarget(winrt::Microsoft::UI::WindowId{ reinterpret_cast<uint64_t>(m_window) }, m_target);
}

// Don't forget to free memory on WM_DESTROY!
LRESULT MyAppWindow::MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
{
    if (WM_DESTROY == message)
    {
        m_micaController = nullptr;
    }

    return base_type::MessageHandler(message, wparam, lparam);
}
