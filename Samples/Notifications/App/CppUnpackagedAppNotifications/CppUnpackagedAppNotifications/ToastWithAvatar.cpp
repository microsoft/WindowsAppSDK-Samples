#include "pch.h"
#include "ToastWithAvatar.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "MainPage.xaml.h""
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

bool ToastWithAvatar::SendToast()
{
    auto path = ExePath();

    winrt::hstring xmlPayload{
        L"<toast>\
                <visual>\
                    <binding template = \"ToastGeneric\">\
                        <image placement = \"appLogoOverride\" src = \"" + path + L"\\Assets\\Square150x150Logo.png\"/>\
                        <text>App Notifications Sample Scenario 1</text>\
                        <text>This is an example message using XML</text>\
                    </binding>\
                </visual>\
                <actions>\
                    <action\
                        content = \"Activate Toast\"\
                        arguments = \"action=activateToast&amp;contentId=351\"\
                        activationType = \"foreground\" />\
                </actions>\
            </toast>" };

    auto toast{ winrt::AppNotification(xmlPayload) };
    toast.Priority(winrt::AppNotificationPriority::High);
    winrt::AppNotificationManager::Default().Show(toast);
    if (toast.Id() == 0)
    {
        return false;
    }

    return true;
}


void ToastWithAvatar::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs  const& notificationActivatedEventArgs)
{
    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().ActivateScenario(L"CppUnpackagedAppNotifications.Scenario1_ToastWithAvatar");
    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(L"NotificationInvoked: Successful invocation from toast!", winrt::Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
}
