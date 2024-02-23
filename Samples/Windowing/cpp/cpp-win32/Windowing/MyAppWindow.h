// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "DesktopWindow.h"

#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>

struct MyAppWindow : DesktopWindow<MyAppWindow>
{
    static const std::wstring ClassName;
    static void RegisterWindowClass();

    MyAppWindow(const winrt::Windows::UI::Composition::Compositor& compositor, const std::wstring& windowTitle);

    LRESULT MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept;

    winrt::Windows::UI::Composition::Visual Root() { return m_target.Root(); }
    void Root(const winrt::Windows::UI::Composition::Visual& visual) { m_target.Root(visual); }

private:
    winrt::Windows::UI::Composition::CompositionTarget m_target{ nullptr };
    winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController m_micaController{ nullptr };
    bool m_isMicaSupported{ false };
};
