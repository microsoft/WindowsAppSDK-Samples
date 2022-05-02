// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "CompositionWindow.h"

namespace winrt
{
    using namespace Microsoft::UI::Composition::SystemBackdrops;
    using namespace Windows::UI::Composition;
}

// static
const std::wstring CompositionWindow::ClassName = L"CompositionWindow";

// static
void CompositionWindow::RegisterWindowClass()
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
    winrt::check_bool(RegisterClassExW(&wcex));
}

// Create the main window and enable MICA
CompositionWindow::CompositionWindow(const winrt::Compositor& compositor, const std::wstring& windowTitle)
{
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    WINRT_ASSERT(!m_window);
    WINRT_VERIFY(
        // Default Window Properties
        CreateWindowExW(
            WS_EX_COMPOSITED, 
            ClassName.c_str(), // declared in CompositionWindow.h and defined above
            windowTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            640, 480, 
            nullptr, 
            nullptr, 
            instance, 
            this
        ));

    // Check that the window was created succesfully
    WINRT_ASSERT(m_window);

    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);

    // The Mica controller needs to set a target with a root to recognize the visual base layer
    m_target = CreateWindowTarget(compositor);

    // Need to set a root before we can enable Mica.
    m_target.Root(compositor.CreateContainerVisual());

    m_systemBackdropController = winrt::MicaController();
    m_isCurrentSystemBackdropSupported = m_systemBackdropController.SetTarget(winrt::Microsoft::UI::WindowId{ reinterpret_cast<uint64_t>(m_window) }, m_target);
}

// Make sure to free memory after the WM_DESTROY signal!
LRESULT CompositionWindow::MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
{
    if (WM_DESTROY == message)
    {
        m_systemBackdropController = nullptr;
    }

    return base_type::MessageHandler(message, wparam, lparam);
}
