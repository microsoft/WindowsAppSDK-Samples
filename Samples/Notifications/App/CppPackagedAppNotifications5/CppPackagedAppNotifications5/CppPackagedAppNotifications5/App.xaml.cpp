#include "pch.h"

#include "App.xaml.h"
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace CppPackagedAppNotifications5;
using namespace CppPackagedAppNotifications5::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    window = make<MainWindow>();

    auto notificationManager{ winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default() };
    const auto token = notificationManager.NotificationInvoked([&](const auto&, const winrt::Microsoft::Windows::AppNotifications::AppNotificationActivatedEventArgs& notificationActivatedEventArgs)
        {
            std::wstring args{ notificationActivatedEventArgs.Argument().c_str() };
            if (args.find(L"activateToast") != std::wstring::npos)
            {
                //MainPage::Current().NotifyUser(L"NotificationInvoked: Successful invocation from toast!", Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
            }

            if (args.find(L"reply") != std::wstring::npos)
            {
                auto input{ notificationActivatedEventArgs.UserInput() };
                auto text{ input.Lookup(L"tbReply") };

                std::wstring message{ L"NotificationInvoked: Successful invocation from toast! [" };
                message.append(text);
                message.append(L"]");

                //MainPage::Current().NotifyUser(message.c_str(), Microsoft::UI::Xaml::Controls::InfoBarSeverity::Informational);
            }
#if 0
            // Check if activated from background by AppNotification
            if (args. == winrt::ExtendedActivationKind::AppNotification)
            {
            }
#endif
            //window.Activate();
            //window.Content().
            //MainPage::Current().
            //rootPage.NotifyUser(L"Toast Activation Received!", InfoBarSeverity::Informational);
            //ProcessNotificationArgs(notificationActivatedEventArgs);
        });

    //notificationManager.Register();
    window.Activate();
}
