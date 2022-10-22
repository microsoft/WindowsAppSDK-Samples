#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "ActivationTrackerHelper.h"
#include <winrt/Microsoft.Windows.AppLifecycle.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Navigation;
using namespace CppWinUiDesktopInstancing;
using namespace CppWinUiDesktopInstancing::implementation;

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
    auto numberOfActivations{ IncrementNumberOfRedirections() };
    window = make<MainWindow>();

    // GetInstances also gets the current instance.
    // Meat and potatoes of redirection.
    auto instances = winrt::Microsoft::Windows::AppLifecycle::AppInstance::GetInstances();

    // Number will be incorrect if instance  (instanceNumber < c_maxNumberOfInstances) is closed.
    // Can be fixed on an OnExit event handler.
    std::wstring windowTitle{L"Instances: "};
    windowTitle.append(std::to_wstring(instances.Size() - 1));
    window.Title(windowTitle);


    // Need activated event args to pass into the redirected instance.
    auto activatedArgs = winrt::Microsoft::Windows::AppLifecycle::AppInstance::GetCurrent().GetActivatedEventArgs();
    bool isActivationRedirected = false;
    if (instances.Size() > c_maxNumberOfInstances)
    {
        // Time to redirect
        // Choose what instance to redirect to.
        int instanceToRedirectTo = (numberOfActivations % c_maxNumberOfInstances);
        isActivationRedirected = true;

        // Need to call RedirectActivationToAsync on the instance to redirect to.
        // Not passing in the instance into RedirectionActivationToAsync.
        instances.GetAt(instanceToRedirectTo).RedirectActivationToAsync(activatedArgs).GetResults();
    }

    if (isActivationRedirected)
    {
        this->Exit();
    }
    else
    {
        window.Activate();
    }
}
