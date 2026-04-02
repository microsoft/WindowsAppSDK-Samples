// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_NumericBadge.xaml.h"
#if __has_include("Scenario1_NumericBadge.g.cpp")
#include "Scenario1_NumericBadge.g.cpp"
#include <winrt/Microsoft.Windows.BadgeNotifications.h>
#endif

using namespace winrt;

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::BadgeNotifications;
}

namespace winrt::cpp_winui::implementation
{
    MainPage Scenario1_NumericBadge::rootPage{ nullptr };

    Scenario1_NumericBadge::Scenario1_NumericBadge()
    {
        InitializeComponent();
        Scenario1_NumericBadge::rootPage = MainPage::Current();
    }

    void Scenario1_NumericBadge::showNumericBadge_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring inputText = inputTextBox().Text();
        if (inputText.empty())
        {
            rootPage.NotifyUser(L"Please enter a number.", InfoBarSeverity::Informational);
            return;
        }

        if (!std::all_of(inputText.begin(), inputText.end(), ::isdigit))
        {
            rootPage.NotifyUser(L"Please enter a valid number.", InfoBarSeverity::Informational);
            return;
        }

        int badgeCount = std::stoi(winrt::to_string(inputText));

        if (badgeCount < 0)
        {
            rootPage.NotifyUser(L"Please enter a positive number.", InfoBarSeverity::Informational);
            return;
        }

        try
        {
            winrt::BadgeNotificationManager::Current().SetBadgeAsCount(badgeCount);
            rootPage.NotifyUser(L"Badge Notification Displayed Successfully.", InfoBarSeverity::Informational);
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"An error occurred: " + ex.message(), InfoBarSeverity::Informational);
        }
    }
}
