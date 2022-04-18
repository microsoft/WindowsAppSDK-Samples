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

        MyCustomEditControl().SetAppWindow(GetAppWindowId());

        Content().PointerPressed({ this, &MainWindow::PointerPressed });
    }

    winrt::WindowId MainWindow::GetAppWindowId()
    {
        winrt::AppWindow appWindow = nullptr;

        // Get the HWND for the XAML Window
        HWND hWnd;
        winrt::Window window = this->try_as<winrt::Window>();
        window.as<IWindowNative>()->get_WindowHandle(&hWnd);

        // Get the WindowId for the HWND
        return GetWindowIdFromWindow(hWnd);
    }

    static winrt::Rect GetElementRect(winrt::UIElement element)
    {
        auto transform = element.TransformToVisual(nullptr);
        auto point = transform.TransformPoint(winrt::Point());
        return winrt::Rect(point, element.ActualSize());
    }

    void MainWindow::PointerPressed(const winrt::IInspectable& /*sender*/, const winrt::PointerRoutedEventArgs& args)
    {
        winrt::Rect contentRect = GetElementRect(MyCustomEditControl());
        if (winrt::RectHelper::Contains(contentRect, args.GetCurrentPoint(nullptr).Position()))
        {
            // The user tapped inside the control. Give it focus.
            MyCustomEditControl().SetInternalFocus();

            // Tell XAML that our custom element has focus, so we don't have two
            // focous elements. That is the extent of our integration with XAML focus.
            MyCustomEditControl().Focus(winrt::FocusState::Programmatic);

            // A more complete custom control would move the caret to the
            // pointer position. It would also provide some way to select
            // text via touch. We do neither in this sample.
        }
        else
        {
            // The user tapped outside the control. Remove focus.
            MyCustomEditControl().RemoveInternalFocus();
        }
    }
}
