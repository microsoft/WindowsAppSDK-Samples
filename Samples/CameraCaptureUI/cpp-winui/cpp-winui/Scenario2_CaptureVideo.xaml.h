// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario2_CaptureVideo.g.h"

namespace winrt::cpp_winui::implementation
{
    struct Scenario2_CaptureVideo : Scenario2_CaptureVideoT<Scenario2_CaptureVideo>
    {
        Scenario2_CaptureVideo();
        winrt::fire_and_forget CaptureVideo_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static cpp_winui::MainPage rootPage;
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct Scenario2_CaptureVideo : Scenario2_CaptureVideoT<Scenario2_CaptureVideo, implementation::Scenario2_CaptureVideo>
    {
    };
}

