#include "pch.h"
#include "NotificationManager.h"
#include "MainPage.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"

#include <map>
#include <functional>

namespace winrt
{
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::Windows::AppNotifications;
}

static const std::map<std::wstring, std::function<void (winrt::AppNotificationActivatedEventArgs const&)>> c_map
{
    { L"1", ToastWithAvatar::NotificationReceived },
    { L"2", ToastWithTextBox::NotificationReceived }
};

NotificationManager::NotificationManager():m_isRegistered(false){}

NotificationManager::~NotificationManager()
{
    if (m_isRegistered)
    {
        winrt::AppNotificationManager::Default().Unregister();
    }
}

void NotificationManager::Init()
{
    auto notificationManager{ winrt::AppNotificationManager::Default() };
    const auto token{ notificationManager.NotificationInvoked([&](const auto&, winrt::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
        {
            winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Notification received", winrt::InfoBarSeverity::Informational);

            if (!DispatchNotification(notificationActivatedEventArgs))
            {
                winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"Unrecognized Toast Originator", winrt::InfoBarSeverity::Error);
            }
        }) };

    winrt::AppNotificationManager::Default().Register();
    m_isRegistered = true;
}

void NotificationManager::ProcessActivationArgs(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    assert(m_isRegistered);

    DispatchNotification(notificationActivatedEventArgs);
    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"App launched from notifications", winrt::InfoBarSeverity::Informational);
}

std::optional<std::wstring> GetScenarioId(std::wstring const& args)
{
    auto tag{ L"scenarioId=" };

    auto scenarioIdStart{ args.find(tag) };
    if (scenarioIdStart == std::wstring::npos)
    {
        return std::nullopt;
    }

    scenarioIdStart += wcslen(tag);

    auto scenarioIdEnd{ args.find(L"&", scenarioIdStart) };

    return args.substr(scenarioIdStart, scenarioIdEnd - scenarioIdStart) ;
}

bool NotificationManager::DispatchNotification(winrt::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto scenarioId{ GetScenarioId(notificationActivatedEventArgs.Argument().c_str()) };
    if (scenarioId.has_value())
    {
        try
        {
            c_map.at(scenarioId.value())(notificationActivatedEventArgs); // Will throw if out of range
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
