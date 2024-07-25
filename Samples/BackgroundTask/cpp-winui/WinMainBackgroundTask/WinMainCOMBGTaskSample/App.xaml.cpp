#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "RegisterForCom.h"
#include "Win32BGClass.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::WinMainCOMBGTaskSample;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinMainCOMBGTaskSample::implementation
{
    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
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
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="e">Details about the launch request and process.</param>
    void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();
        window.Activate();
    }

}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    OutputDebugString(L"This is command line args: ");
    OutputDebugString(lpCmdLine);

    if (std::wcsncmp(lpCmdLine, RegisterForCom::RegisterForComToken, sizeof(RegisterForCom::RegisterForComToken)) == 0)
    {
        RegisterForCom comRegister;
        comRegister.RegisterAndWait(__uuidof(Win32BGTask));
        MSG msg;

        while (-1 != GetMessage(&msg, NULL, 0, 0) &&
            WM_QUIT != msg.message)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    else
    {
        // put your fancy code somewhere here
        ::winrt::Microsoft::UI::Xaml::Application::Start(
            [](auto&&)
            {
                ::winrt::make<::winrt::WinMainCOMBGTaskSample::implementation::App>();
            });
    }

    return 0;
}