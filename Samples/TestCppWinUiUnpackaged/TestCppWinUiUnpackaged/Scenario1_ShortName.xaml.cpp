// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "winrt\Microsoft.Windows.System.Power.h"
#include "Scenario1_ShortName.xaml.h"
#if __has_include("Scenario1_ShortName.g.cpp")
#include "Scenario1_ShortName.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace winrt::Microsoft::Windows::System::Power;
}

namespace winrt::TestCppWinUiUnpackaged::implementation
{
    MainPage Scenario1_ShortName::rootPage{ nullptr };

    Scenario1_ShortName::Scenario1_ShortName()
    {
        InitializeComponent();
        Scenario1_ShortName::rootPage = MainPage::Current();
    }
    
    void Scenario1_ShortName::DisplayStatus_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto dispStatus = winrt::PowerManager::DisplayStatus();
        switch (dispStatus)
        {
            case 0:
                rootPage.NotifyUser(L"The display is off.", InfoBarSeverity::Success);
                break;

            case 1:
                rootPage.NotifyUser(L"The display is on.", InfoBarSeverity::Success);
                break;

            case 2:
                rootPage.NotifyUser(L"The display is dimmed.", InfoBarSeverity::Success);
                break;

            default:
                rootPage.NotifyUser(L"The display is in an undefined state.", InfoBarSeverity::Success);
        }
    }
}
