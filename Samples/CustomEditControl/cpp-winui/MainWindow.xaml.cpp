#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

// This include file is needed for the XAML Native Window Interop.
#include "microsoft.ui.xaml.window.h"

namespace winrt
{
    using namespace ::winrt::Microsoft::UI;
    using namespace ::winrt::Microsoft::UI::Windowing;
    using namespace ::winrt::Microsoft::UI::Xaml;
    using namespace ::winrt::Microsoft::UI::Xaml::Input;
    using namespace ::winrt::Windows::Foundation;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CustomEditControlWinAppSDK::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        MyCustomEditControl().SetAppWindow(GetAppWindow());
    }

    winrt::WindowId MainWindow::GetAppWindow()
    {
        winrt::AppWindow appWindow = nullptr;

        // Get the HWND for the XAML Window
        HWND hWnd;
        winrt::Window window = this->try_as<winrt::Window>();
        window.as<IWindowNative>()->get_WindowHandle(&hWnd);

        // Get the WindowId for the HWND
        return GetWindowIdFromWindow(hWnd);
    }



    
}
