// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "Helpers.h"
#include "ChildWindow.h"
#include <windowsx.h>

ChildWindow::ChildWindow(HWND parentWindow, TextRenderer* textRenderer, DWORD style) :
    m_textRenderer{ textRenderer }
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
        //wcex.hIcon
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        //wcex.lpszMenuName
        wcex.lpszClassName = L"wc_child";
        //wcex.hIconSm

        classAtom = RegisterClassExW(&wcex);
        THROW_LAST_ERROR_IF(classAtom == 0);
    }

    // Create the window.
    CreateWindowExW(
        0,
        MAKEINTATOM(classAtom),
        L"",
        WS_CHILD | style,
        0, 0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        parentWindow,
        nullptr,
        g_hInstance,
        this
        );

    // If successful, the WM_CREATE message handler
    // should have set the m_hwnd member.
    THROW_LAST_ERROR_IF_NULL(m_hwnd);

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

ChildWindow::~ChildWindow()
{
    DestroyWindow(m_hwnd);
}

void ChildWindow::Resize(int pixelWidth, int pixelHeight)
{
    Move(0, 0, pixelWidth, pixelHeight);
}

void ChildWindow::Move(int pixelLeft, int pixelTop, int pixelWidth, int pixelHeight)
{
    SetWindowPos(m_hwnd, nullptr, pixelLeft, pixelTop, pixelWidth, pixelHeight, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT ChildWindow::Handle_Create(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_hwnd = hwnd;
    return 0;
}

LRESULT ChildWindow::Handle_Paint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    auto hdc = wil::BeginPaint(hwnd, &ps);
    OnPaint(hdc.get(), ps.rcPaint);
    return 0;
}

LRESULT ChildWindow::Handle_KeyDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (OnKeyDown(static_cast<uint32_t>(wParam)))
    {
        return 0;
    }
    else
    {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

LRESULT ChildWindow::Handle_LButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Send the message to the parent window if it's not handled by the current window.
    if (OnLeftButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
    {
        return 0;
    }
    else
    {
        return SendMessageW(GetParent(m_hwnd), message, wParam, lParam);
    }
}

LRESULT ChildWindow::Handle_LButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnLeftButtonUp();
    return 0;
}

LRESULT ChildWindow::Handle_MouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}


LRESULT ChildWindow::Handle_VScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnVSCroll(LOWORD(wParam));
    return 0;
}

LRESULT ChildWindow::Handle_MouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnMouseWheel(static_cast<int16_t>(HIWORD(wParam)));
    return 0;
}

LRESULT ChildWindow::Handle_Size(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    m_pixelWidth = clientRect.right;
    m_pixelHeight = clientRect.bottom;

    OnSize();
    return 0;
}

LRESULT ChildWindow::Handle_Command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (lParam != 0)
    {
        return OnControlCommand(/*controlHandle*/ reinterpret_cast<HWND>(lParam), /*controlId*/ LOWORD(wParam), /*notificationCode*/ HIWORD(wParam));
    }
    else
    {
        OnMenuCommand(LOWORD(wParam));
        return 0;
    }
}

LRESULT ChildWindow::Handle_SetFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnFocus(true);
    return 0;
}

LRESULT ChildWindow::Handle_KillFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OnFocus(false);
    return 0;
}

LRESULT CALLBACK ChildWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (message)
    {
    case WM_SIZE:
        return CallMessageProc(&Handle_Size, hwnd, message, wParam, lParam);

    case WM_PAINT:
        return CallMessageProc(&Handle_Paint, hwnd, message, wParam, lParam);

    case WM_LBUTTONDOWN:
        return CallMessageProc(&Handle_LButtonDown, hwnd, message, wParam, lParam);

    case WM_LBUTTONUP:
        return CallMessageProc(&Handle_LButtonUp, hwnd, message, wParam, lParam);

    case WM_MOUSEMOVE:
        return CallMessageProc(&Handle_MouseMove, hwnd, message, wParam, lParam);

    case WM_VSCROLL:
        return CallMessageProc(&Handle_VScroll, hwnd, message, wParam, lParam);

    case WM_MOUSEWHEEL:
        return CallMessageProc(&Handle_MouseWheel, hwnd, message, wParam, lParam);

    case WM_COMMAND:
        return CallMessageProc(&Handle_Command, hwnd, message, wParam, lParam);

    case WM_KEYDOWN:
        return CallMessageProc(&Handle_KeyDown, hwnd, message, wParam, lParam);

    case WM_SETFOCUS:
        return CallMessageProc(&Handle_SetFocus, hwnd, message, wParam, lParam);

    case WM_KILLFOCUS:
        return CallMessageProc(&Handle_KillFocus, hwnd, message, wParam, lParam);

    case WM_CREATE:
        return CallOnCreateProc(&Handle_Create, hwnd, message, wParam, lParam);

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}

void ScrollableChildWindow::SetPixelScrollHeight(int pixelHeight)
{
    m_pixelScrollHeight = pixelHeight;

    int maxScrollTop = GetMaxScrollTop();
    if (m_pixelScrollTop > maxScrollTop)
    {
        m_pixelScrollTop = maxScrollTop;
    }

    UpdateScrollbar();
}

int ScrollableChildWindow::GetScrollTrackPos()
{
    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_TRACKPOS;
    GetScrollInfo(GetHandle(), SB_VERT, &info);
    return info.nTrackPos;
}

void ScrollableChildWindow::UpdateScrollbar()
{
    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    info.nMax = GetPixelScrollHeight();
    info.nPage = GetPixelHeight();
    info.nPos = GetPixelScrollTop();

    SetScrollInfo(GetHandle(), SB_VERT, &info, /*redraw*/ true);
}

void ScrollableChildWindow::OnSize()
{
    UpdateScrollbar();
}

int ScrollableChildWindow::GetMaxScrollTop() const noexcept
{
    return std::max(0, m_pixelScrollHeight - GetPixelHeight());
}

void ScrollableChildWindow::SetPixelScrollTop(int newPos)
{
    newPos = std::max(newPos, 0);
    newPos = std::min(newPos, GetMaxScrollTop());

    int const oldPos = m_pixelScrollTop;

    if (newPos != oldPos)
    {
        m_pixelScrollTop = newPos;

        SCROLLINFO info = {};
        info.cbSize = sizeof(info);
        info.fMask = SIF_DISABLENOSCROLL | SIF_POS;
        info.nPos = newPos;

        SetScrollInfo(GetHandle(), SB_VERT, &info, true);
        ScrollWindowEx(GetHandle(), 0, oldPos - newPos, nullptr, nullptr, nullptr, nullptr, SW_INVALIDATE);
    }
}

int ScrollableChildWindow::GetLineHeight() const noexcept
{
    return static_cast<int>(ceilf(20.0f * GetTextRenderer()->GetDpiScale()));
}

void ScrollableChildWindow::OnVSCroll(uint32_t requestId)
{
    int newPos = m_pixelScrollTop;

    switch (requestId)
    {
    case SB_BOTTOM:
        newPos = INT_MAX;
        break;

    case SB_TOP:
        newPos = 0;
        break;

    case SB_LINEUP:
        newPos -= GetLineHeight();
        break;

    case SB_LINEDOWN:
        newPos += GetLineHeight();
        break;

    case SB_PAGEUP:
        newPos -= GetPixelHeight();
        break;

    case SB_PAGEDOWN:
        newPos += GetPixelHeight();
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        newPos = GetScrollTrackPos();
        break;
    }

    SetPixelScrollTop(newPos);
}

void ScrollableChildWindow::OnMouseWheel(int distance)
{
    SetPixelScrollTop(m_pixelScrollTop - distance);
}
