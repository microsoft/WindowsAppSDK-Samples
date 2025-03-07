// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

namespace winrt::MyApp::implementation
{
    struct MyAppWindow : winrt::implements<MyAppWindow, winrt::Windows::Foundation::IInspectable>
    {
        MyAppWindow(
            const winrt::Microsoft::UI::Dispatching::DispatcherQueue & queue,
            const winrt::Windows::UI::Composition::Compositor& compositor,
            const std::wstring& windowTitle);

        winrt::Windows::UI::Composition::Visual Root() { return m_target.Root(); }
        void Root(const winrt::Windows::UI::Composition::Visual& visual) { m_target.Root(visual); }

    private:
        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_queue{ nullptr };
        winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{ nullptr };

        winrt::Windows::UI::Composition::CompositionTarget m_target{ nullptr };
        winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController m_micaController{ nullptr };
        bool m_isMicaSupported{ false };

        void Window_Destroying(
            const winrt::Microsoft::UI::Windowing::AppWindow& sender,
            const winrt::Windows::Foundation::IInspectable & args);
    };

} // namespace winrt::MyApp::implementation
