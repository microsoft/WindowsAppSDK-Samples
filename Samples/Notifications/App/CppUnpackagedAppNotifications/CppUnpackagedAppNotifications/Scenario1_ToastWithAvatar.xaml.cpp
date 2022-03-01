// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario1_ToastWithAvatar::rootPage{ nullptr };

    Scenario1_ToastWithAvatar::Scenario1_ToastWithAvatar()
    {
        InitializeComponent();
        Scenario1_ToastWithAvatar::rootPage = MainPage::Current();
    }
    
    void Scenario1_ToastWithAvatar::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"Toast sent successfully!", InfoBarSeverity::Success);
    }
}
