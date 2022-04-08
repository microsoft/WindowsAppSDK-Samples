// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MicaWindow.h"

namespace winrt
{
    using namespace Microsoft::UI::Composition::SystemBackdrops;
    using namespace Windows::UI::Composition;
}

// static
const std::wstring MicaWindow::ClassName = L"MicaWindow";

// static
void MicaWindow::RegisterWindowClass()
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

// Create the main window and enable MICA
MicaWindow::MicaWindow(const winrt::Compositor& compositor, const std::wstring& windowTitle)
{
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    WINRT_ASSERT(!m_window); // check that window is not initialized
    WINRT_VERIFY(
        // Window Properties
        CreateWindowExW(
            WS_EX_COMPOSITED,
            ClassName.c_str(), // declared in MicaWindow.h and defined above
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

    // Check that the window was created succesfully
    WINRT_ASSERT(m_window);

    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);

    // The Mica controller needs to set a target with a root to recognize the visual base layer
    m_target = CreateWindowTarget(compositor);

    // Need to set a root before we can enable Mica.
    m_target.Root(compositor.CreateContainerVisual());

    m_micaController = winrt::MicaController();
    m_isMicaSupported = m_micaController.SetTarget(winrt::Microsoft::UI::WindowId{ reinterpret_cast<uint64_t>(m_window) }, m_target);
}

// Don't forget to free memory on WM_DESTROY!
LRESULT MicaWindow::MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
{
    if (WM_DESTROY == message)
    {
        m_micaController = nullptr;
    }

    return base_type::MessageHandler(message, wparam, lparam);
}
