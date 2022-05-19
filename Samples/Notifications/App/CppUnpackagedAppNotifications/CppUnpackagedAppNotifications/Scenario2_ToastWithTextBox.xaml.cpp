// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "Scenario2_ToastWithTextBox.xaml.h"
#if __has_include("Scenario2_ToastWithTextBox.g.cpp")
#include "Scenario2_ToastWithTextBox.g.cpp"
#endif
#include "Notifications\ToastWithTextBox.h"
#include "NotifyUser.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::AppNotifications;
}

namespace winrt::CppUnpackagedAppNotifications::implementation
{
    MainPage Scenario2_ToastWithTextBox::rootPage{ nullptr };

    Scenario2_ToastWithTextBox::Scenario2_ToastWithTextBox()
    {
        InitializeComponent();

        Scenario2_ToastWithTextBox::rootPage = MainPage::Current();
    }

    void Scenario2_ToastWithTextBox::SendToast_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (ToastWithTextBox::SendToast())
        {
            NotifyUser::ToastSentSuccessfully();
        }
        else
        {
            NotifyUser::CouldNotSendToast();
        }
    }
}
