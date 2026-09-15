// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Scenario1_CapturePhoto.g.h"

namespace winrt::cpp_winui::implementation
{
    struct Scenario1_CapturePhoto : Scenario1_CapturePhotoT<Scenario1_CapturePhoto>
    {
        Scenario1_CapturePhoto();
        winrt::fire_and_forget CapturePhoto_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ClearMessage_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    private:
        static cpp_winui::MainPage rootPage;
    };
}

namespace winrt::cpp_winui::factory_implementation
{
    struct Scenario1_CapturePhoto : Scenario1_CapturePhotoT<Scenario1_CapturePhoto, implementation::Scenario1_CapturePhoto>
    {
    };
}
