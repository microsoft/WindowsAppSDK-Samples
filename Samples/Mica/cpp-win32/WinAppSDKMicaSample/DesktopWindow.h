// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <Windows.UI.Composition.Interop.h>

#include <winrt/Windows.UI.Composition.Desktop.h>

template <typename T>
struct DesktopWindow
{
    using base_type = DesktopWindow<T>;
    HWND m_window = nullptr;

    static T* GetThisFromHandle(const HWND window) noexcept
    {
        return reinterpret_cast<T*>(GetWindowLongPtr(window, GWLP_USERDATA));
    }

    static LRESULT __stdcall WndProc(const HWND window, const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
    {
        WINRT_ASSERT(window);

        if (WM_NCCREATE == message)
        {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
            T* that = static_cast<T*>(cs->lpCreateParams);
            WINRT_ASSERT(that);
            WINRT_ASSERT(!that->m_window);
            that->m_window = window;
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
        }
        else if (T* that = GetThisFromHandle(window))
        {
            return that->MessageHandler(message, wparam, lparam);
        }

        return DefWindowProc(window, message, wparam, lparam);
    }

    LRESULT MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
    {
        if (WM_DESTROY == message)
        {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(m_window, message, wparam, lparam);
    }

    winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget CreateWindowTarget(const winrt::Windows::UI::Composition::Compositor& compositor)
    {
        namespace abi = ABI::Windows::UI::Composition::Desktop;

        auto interop = compositor.as<abi::ICompositorDesktopInterop>();
        winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{ nullptr };
        winrt::check_hresult(interop->CreateDesktopWindowTarget(m_window, true, reinterpret_cast<abi::IDesktopWindowTarget**>(winrt::put_abi(target))));
        return target;
    }
};
