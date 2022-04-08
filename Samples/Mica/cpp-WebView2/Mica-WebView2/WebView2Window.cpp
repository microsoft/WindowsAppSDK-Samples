// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "WebView2Window.h"

#include <wrl.h>

namespace winrt
{
    using namespace Windows::UI::Composition;
}

WebView2Window::WebView2Window(const winrt::Compositor& compositor, const std::wstring& windowTitle) :
    CompositionWindow(compositor, windowTitle)
{
    InitializeWebView2();
}

LRESULT WebView2Window::MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
{
    if (WM_SIZE == message && nullptr != m_webView2Controller)
    {
        RECT bounds;
        GetClientRect(m_window, &bounds);
        m_webView2Controller->put_Bounds(bounds);
    }

    return CompositionWindow::MessageHandler(message, wparam, lparam);
}

void WebView2Window::InitializeWebView2()
{
    CreateCoreWebView2Environment(Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
        [this](HRESULT createEnvironmentResult, ICoreWebView2Environment* webView2Environment) -> HRESULT
        {
            if (S_OK == createEnvironmentResult && nullptr != webView2Environment)
            {
                // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd.
                webView2Environment->CreateCoreWebView2Controller(m_window, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [this](HRESULT createControllerResult, ICoreWebView2Controller* controller) -> HRESULT
                    {
                        if (S_OK == createControllerResult && controller != nullptr)
                        {
                            // Web View 2 has a its own seperate controller
                            m_webView2Controller = controller;
                            m_webView2Controller->get_CoreWebView2(&m_webView2Window);

                            // Resize WebView to fit the bounds of the parent window.
                            RECT bounds;
                            GetClientRect(m_window, &bounds);
                            m_webView2Controller->put_Bounds(bounds);

                            // Make the WebView2 transparent.
                            wil::com_ptr<ICoreWebView2Controller2> controller2 = m_webView2Controller.query<ICoreWebView2Controller2>();
                            controller2->put_DefaultBackgroundColor({ 0, 0, 0, 0 });

                            // Schedule an async task to navigate to some translucent HTML.
                            m_webView2Window->NavigateToString(L"\
                            <html>\
                                <head>\
                                    <style>\
                                    .box\
                                    {\
                                        font-family: sans-serif;\
                                        font-size: 32px;\
                                    }\
                                    </style>\
                                </head>\
                                <body>\
                                    <div class=\"box\">\
                                        <div style=\"color: rgba(0, 0, 0, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(0, 0, 255, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(255, 0, 0, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(0, 255, 0, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(0, 255, 255, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(255, 0, 255, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(255, 255, 0, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                        <div style=\"color: rgba(255, 255, 255, 1.0);\">Welcome to WebView2 with Mica!</div><br/>\
                                    </div>\
                                </body>\
                            </html>");
                        }

                        return S_OK;
                    }).Get());
            }

            return S_OK;
        }).Get());
}
