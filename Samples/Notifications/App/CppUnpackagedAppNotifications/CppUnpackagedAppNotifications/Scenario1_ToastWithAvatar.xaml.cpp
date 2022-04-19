// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario1_ToastWithAvatar.xaml.h"
#if __has_include("Scenario1_ToastWithAvatar.g.cpp")
#include "Scenario1_ToastWithAvatar.g.cpp"
#endif

#include <sstream>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.Storage.h>
#include "ToastWithAvatar.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <windows.ui.popups.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;

#include <windows.h>
#include <string>
#include <iostream>

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
        if (ToastWithAvatar::SendToast())
        {
            rootPage.NotifyUser(L"Toast sent successfully!", InfoBarSeverity::Success);
        }
        else
        {
            rootPage.NotifyUser(L"Could not send toast", InfoBarSeverity::Error);
        }
    }
}
