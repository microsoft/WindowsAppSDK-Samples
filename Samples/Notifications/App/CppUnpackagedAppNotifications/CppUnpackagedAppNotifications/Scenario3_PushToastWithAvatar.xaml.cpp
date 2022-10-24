// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario3_PushToastWithAvatar.xaml.h"
#if __has_include("Scenario3_PushToastWithAvatar.g.cpp")
#include "Scenario3_PushToastWithAvatar.g.cpp"
#endif
#include "NotifyUser.h"
#include "Notifications\ToastWithAvatar.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::AppNotifications;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario3_PushToastWithAvatar::rootPage{ nullptr };

    Scenario3_PushToastWithAvatar::Scenario3_PushToastWithAvatar()
    {
        InitializeComponent();

        Scenario3_PushToastWithAvatar::rootPage = MainPage::Current();
    }

    void Scenario3_PushToastWithAvatar::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (ToastWithAvatar::SendToast())
        {
            NotifyUser::ToastSentSuccessfully();
        }
        else
        {
            NotifyUser::CouldNotSendToast();
        }
    }
}
