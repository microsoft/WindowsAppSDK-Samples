#include "pch.h"
#include "NotifyUser.h"
#include "MainPage.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace winrt::CppUnpackagedAppNotifications::implementation;
}

void NotifyUser::CouldNotSendToast()
{
    winrt::MainPage::Current().NotifyUser(L"Could not send toast", winrt::InfoBarSeverity::Error);
}

void NotifyUser::ToastSentSuccessfully()
{
    winrt::MainPage::Current().NotifyUser(L"Toast sent successfully!", winrt::InfoBarSeverity::Success);
}

void NotifyUser::AppLaunchedFromNotification()
{
    winrt::MainPage::Current().NotifyUser(L"App launched from notifications", winrt::InfoBarSeverity::Informational);
}

void NotifyUser::NotificationReceived()
{
    winrt::MainPage::Current().NotifyUser(L"Notification received", winrt::InfoBarSeverity::Informational);
}

void NotifyUser::UnrecognizedToastOriginator()
{
    winrt::MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", winrt::InfoBarSeverity::Error);
}
