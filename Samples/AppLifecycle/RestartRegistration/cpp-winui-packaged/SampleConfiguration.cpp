// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h" 

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Windows::Foundation::Collections;
}

namespace winrt::cpp_winui_packaged
{
    IVector<Scenario> implementation::MainPage::scenariosInner = single_threaded_observable_vector<Scenario>(
        {
            Scenario{ L"Update reboot registration", hstring(name_of<cpp_winui_packaged::S1_UpdateReboot>())},
            Scenario{ L"Crash recovery registration", hstring(name_of<cpp_winui_packaged::S2_CrashRecovery>())},
            Scenario{ L"Reboot type detection", hstring(name_of<cpp_winui_packaged::S3_RebootTypeDetection>())}
        });

    hstring SampleConfig::FeatureName{ L"cpp_winui_packaged" };
    ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
}
