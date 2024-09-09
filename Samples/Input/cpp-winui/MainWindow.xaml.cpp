// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "MainWindow.xaml.h"
#include "winrt/Microsoft.UI.Xaml.Input.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::cpp_winui::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        LayoutRoot().KeyDown({ this, &MainWindow::OnKeyDownHandler });
        LayoutRoot().KeyUp({ this, &MainWindow::OnKeyUpHandler });
        LayoutRoot().CharacterReceived({ this, &MainWindow::OnCharacterReceivedHandler });
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::OnKeyDownHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
    {
        hstring keyString = L"Key Down: " + to_hstring(static_cast<int>(e.Key()));
        KeyDownText().Text(keyString);
    }

    void MainWindow::OnKeyUpHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
    {
        hstring keyString = L"Key Up: " + to_hstring(static_cast<int>(e.Key()));
        KeyUpText().Text(keyString);
    }

    void MainWindow::OnCharacterReceivedHandler(IInspectable const& sender, Microsoft::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& e)
    {
        wchar_t character = static_cast<wchar_t>(e.Character());
        std::wstring charString(1, character);
        CharacterReceivedText().Text(L"Character Received: " + charString);
    }

    void MainWindow::OnCtrlRInvoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& sender, Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs e)
    {
        AcceleratorInvokedText().Text(L"Accelerator Invoked: Ctrl+R");
        e.Handled(true);
    }

    void MainWindow::MyButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        MyButton().Content(box_value(L"Clicked"));
    }
}
