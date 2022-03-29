#include "pch.h"
#include "ToastWithTextBox.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "MainPage.xaml.h""
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include "Utils.h"

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Microsoft::Windows::AppNotifications;
}

bool ToastWithTextBox::SendToast()
{
    winrt::hstring xmlPayload{
        L"<toast>\
                <visual>\
                    <binding template = \"ToastGeneric\">\
                        <image placement = \"appLogoOverride\" src = \"" + Utils::GetFullPathToAsset(L"Square150x150Logo.png") + L"\"/>\
                        <text>App Notifications Sample Scenario 2</text>\
                        <text>This is an example message using XML</text>\
                    </binding>\
                </visual>\
                <actions>\
                    <input\
                        id = \"tbReply\"\
                        type = \"text\"\
                        placeHolderContent = \"Type a reply\"/>\
                    <action\
                        content = \"Reply\"\
                        arguments = \"action=reply&amp;contentId=351\"\
                        activationType = \"background\" />\
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

void ToastWithTextBox::NotificationReceived(winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs const& notificationActivatedEventArgs)
{
    auto input{ notificationActivatedEventArgs.UserInput() };
    auto text{ input.Lookup(L"tbReply") };

    std::wstring message{ L"NotificationInvoked: Successful invocation from toast! [" };
    message.append(text);
    message.append(L"]");

    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().ActivateScenario(L"CppUnpackagedAppNotifications.Scenario2_ToastWithTextBox");
    winrt::CppUnpackagedAppNotifications::implementation::MainPage::Current().NotifyUser(message.c_str(), winrt::Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
}
