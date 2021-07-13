// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "Resource.h"
#include "MainWindow.h"
#include "Scenarios.h"

MainWindow::MainWindow(int nShowCmd)
{
    // Register the window class if we haven't already.
    static ATOM classAtom = 0;
    if (classAtom == 0)
    {
        WNDCLASSEXW wcex = {};

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        //wcex.cbClsExtra = 0;
        //wcex.cbWndExtra = 0;
        wcex.hInstance = g_hInstance;
        wcex.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(ID_MAIN_MENU);
        wcex.lpszClassName = L"wc_main";
        wcex.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP_SMALL));

        classAtom = RegisterClassExW(&wcex);
        THROW_LAST_ERROR_IF(classAtom == 0);
    }

    // Create the window.
    CreateWindowExW(
        0,
        MAKEINTATOM(classAtom),
        GetAppTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        nullptr,
        nullptr,
        g_hInstance,
        this
        );

    // If successful, the WM_CREATE message handler
    // should have set the m_hwnd member.
    THROW_LAST_ERROR_IF_NULL(m_hwnd);

    ShowWindow(m_hwnd, nShowCmd);
    UpdateWindow(m_hwnd);
}

LRESULT MainWindow::OnCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_hwnd = hwnd;
    m_textRenderer = new TextRenderer{ hwnd };
    m_activeScenarioId = ID_MENUITEM_INTRO;
    m_activeScenarioWindow = GetScenarioWindow(m_activeScenarioId);
    CheckMenuItem(GetMenu(hwnd), m_activeScenarioId, MF_CHECKED);
    return 0;
}

void MainWindow::ResizeScenarioWindow()
{
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_activeScenarioWindow->Resize(rect.right, rect.bottom);
}

void MainWindow::SetScenario(uint32_t menuItemId)
{
    if (menuItemId != m_activeScenarioId)
    {
        auto newWindow = GetScenarioWindow(menuItemId);
        if (newWindow != nullptr)
        {
            ShowWindow(m_activeScenarioWindow->GetHandle(), SW_HIDE);
            m_activeScenarioWindow = newWindow;
            ResizeScenarioWindow();

            // Uncheck the old scenario's menu item and check the new one.
            auto menuHandle = GetMenu(m_hwnd);
            CheckMenuItem(menuHandle, m_activeScenarioId, MF_UNCHECKED);
            CheckMenuItem(menuHandle, menuItemId, MF_CHECKED);
            m_activeScenarioId = menuItemId;
        }
    }
}

LRESULT MainWindow::OnSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ResizeScenarioWindow();
    return 0;
}

LRESULT MainWindow::OnFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SetFocus(m_activeScenarioWindow->GetHandle());
    return 0;
}

LRESULT MainWindow::OnCommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    uint32_t menuItemId = LOWORD(wParam);

    switch (menuItemId)
    {
    case ID_MENUITEM_FILE_EXIT:
        PostQuitMessage(0);
        break;

    default:
        SetScenario(menuItemId);
        break;
    }
    return 0;
}

LRESULT MainWindow::OnDpiChange(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT dpi = LOWORD(wParam);
    RECT newRect = *reinterpret_cast<RECT const*>(lParam);

    m_textRenderer->SetDpiScale(dpi * (1.0f / 96));

    SetWindowPos(
        hwnd,
        nullptr,
        newRect.left,
        newRect.top,
        newRect.right - newRect.left,
        newRect.bottom - newRect.top,
        SWP_NOACTIVATE | SWP_NOZORDER
        );

    return 0;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (message)
    {
    case WM_SIZE:
        return CallMessageProc(&OnSize, hwnd, message, wParam, lParam);

    case WM_COMMAND:
        return CallMessageProc(&OnCommand, hwnd, message, wParam, lParam);

    case WM_SETFOCUS:
        return CallMessageProc(&OnFocus, hwnd, message, wParam, lParam);

    case WM_DPICHANGED:
        return CallMessageProc(&OnDpiChange, hwnd, message, wParam, lParam);

    case WM_CREATE:
        return CallOnCreateProc(&OnCreate, hwnd, message, wParam, lParam);

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}

ChildWindow* MainWindow::GetScenarioWindow(uint32_t menuItemId)
{
    auto& scenarioPtr = m_scenariosById[menuItemId];
    if (scenarioPtr == nullptr)
    {
        switch (menuItemId)
        {
        case ID_MENUITEM_INTRO:
            scenarioPtr = CreateScenario_BasicTextLayout(m_hwnd, m_textRenderer.get());
            break;

        case ID_MENUITEM_OPTICAL_SIZE:
            scenarioPtr = CreateScenario_OpticalSize(m_hwnd, m_textRenderer.get());
            break;

        case ID_MENUITEM_SYSTEM_FONT_COLLECTION_TYPOGRAPHIC:
            scenarioPtr = CreateScenario_SystemFontCollectionTypographic(m_hwnd, m_textRenderer.get());
            break;

        case ID_MENUITEM_SYSTEM_FONT_COLLECTION_WSS:
            scenarioPtr = CreateScenario_SystemFontCollectionWeightStretchStyle(m_hwnd, m_textRenderer.get());
            break;

        case ID_MENUITEM_CUSTOM_FONT_COLLECTION:
            scenarioPtr = CreateScenario_CustomFontCollection(m_hwnd, m_textRenderer.get());
            break;
        }
    }
    return scenarioPtr.get();
}
