// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "TextRenderer.h"

class ChildWindow
{
public:
    ChildWindow(HWND parentWindow, TextRenderer* textRenderer, DWORD style = 0);
    virtual ~ChildWindow();

    HWND GetHandle() const noexcept
    {
        return m_hwnd;
    }

    TextRenderer* GetTextRenderer() const noexcept
    {
        return m_textRenderer.get();
    }

    void Resize(int pixelWidth, int pixelHeight);
    void Move(int pixelLeft, int pixelTop, int pixelWidth, int pixelHeight);

    int GetPixelWidth() const noexcept
    {
        return m_pixelWidth;
    }

    int GetPixelHeight() const noexcept
    {
        return m_pixelHeight;
    }

protected:

    // Window message handlers.
    virtual void OnPaint(HDC hdc, RECT invalidRect) {}
    virtual bool OnKeyDown(uint32_t keyCode) { return false; }
    virtual bool OnLeftButtonDown(int x, int y) { return false; }
    virtual void OnLeftButtonUp() {}
    virtual void OnMouseMove(int x, int y) {}
    virtual void OnVSCroll(uint32_t requestId) {}
    virtual void OnMouseWheel(int distance) {}
    virtual void OnSize() {}
    virtual void OnFocus(bool haveFocus) {}
    virtual void OnMenuCommand(uint32_t id) {}
    virtual LRESULT OnControlCommand(HWND controlWindow, uint32_t controlId, uint32_t notificationCode) { return 0; }

private:
    LRESULT Handle_Create(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_Paint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_KeyDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_LButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_LButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_MouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_VScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_MouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_Size(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_Command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_SetFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Handle_KillFocus(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

    // The following members are initialized by OnCreate.
    HWND m_hwnd = nullptr;
    wil::com_ptr<TextRenderer> m_textRenderer;

    // The following members are updated in response to WM_SIZE before OnSize is called.
    int m_pixelWidth = 0;
    int m_pixelHeight = 0;
};

class ScrollableChildWindow : public ChildWindow
{
public:
    ScrollableChildWindow(HWND parentWindow, TextRenderer* textRenderer) :
        ChildWindow{ parentWindow, textRenderer, WS_VSCROLL }
    {
    }

    virtual void OnSize();

protected:
    void SetPixelScrollHeight(int pixelHeight);

    int GetPixelScrollHeight() const noexcept
    {
        return m_pixelScrollHeight;
    }

    int GetPixelScrollTop() const noexcept
    {
        return m_pixelScrollTop;
    }

    void SetPixelScrollTop(int newScrollPos);

    void OnVSCroll(uint32_t requestId) override;
    void OnMouseWheel(int distance) override;

private:
    int GetMaxScrollTop() const noexcept;
    int GetLineHeight() const noexcept;
    int GetScrollTrackPos();
    void UpdateScrollbar();

    int m_pixelScrollHeight = 0;
    int m_pixelScrollTop = 0;
};

//
// Helpers for forwarding window messages to instance methods.
//
template<class T>
using MessageProc = LRESULT(T::*)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Calls instance method and catches exceptions.
template<class T>
LRESULT CallMessageProcImpl(T* obj, MessageProc<T> proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    try
    {
        return (obj->*proc)(hwnd, message, wParam, lParam);
    }
    catch (...)
    {
        std::terminate();
    }
    return 0;
}

//
// Called on WM_CREATE or WM_NCCREATE.
// Sets the instance pointer and calls class-specific initialization method.
//
template<class T>
LRESULT CallOnCreateProc(MessageProc<T> proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    auto obj = static_cast<T*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(obj));

    return CallMessageProcImpl(obj, proc, hwnd, message, wParam, lParam);
}

//
// Called on any other message.
// Gets the instance pointer and calls class-specific method.
//
template<class T>
LRESULT CallMessageProc(MessageProc<T> proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    auto obj = reinterpret_cast<T*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (obj == nullptr)
    {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    return CallMessageProcImpl(obj, proc, hwnd, message, wParam, lParam);
}

