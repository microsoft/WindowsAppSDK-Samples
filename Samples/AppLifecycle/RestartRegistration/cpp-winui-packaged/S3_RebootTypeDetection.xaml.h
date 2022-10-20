// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "S3_RebootTypeDetection.g.h"

namespace winrt::cpp_winui_packaged::implementation
{
    struct S3_RebootTypeDetection : S3_RebootTypeDetectionT<S3_RebootTypeDetection>
    {
        S3_RebootTypeDetection();
        void Crash_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Restart_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        void SetRestartTypeText();
    };
}

namespace winrt::cpp_winui_packaged::factory_implementation
{
    struct S3_RebootTypeDetection : S3_RebootTypeDetectionT<S3_RebootTypeDetection, implementation::S3_RebootTypeDetection>
    {
    };
}
