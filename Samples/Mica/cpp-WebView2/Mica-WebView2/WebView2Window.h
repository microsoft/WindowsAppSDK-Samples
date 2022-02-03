// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompositionWindow.h"

#include <wil/com.h>
#include <WebView2.h>

struct WebView2Window : CompositionWindow
{
    WebView2Window(const winrt::Windows::UI::Composition::Compositor& compositor, const std::wstring& windowTitle);

    LRESULT MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept override;

private:
    void InitializeWebView2();

    wil::com_ptr<ICoreWebView2Controller> m_webView2Controller;
    wil::com_ptr<ICoreWebView2> m_webView2Window;
};
