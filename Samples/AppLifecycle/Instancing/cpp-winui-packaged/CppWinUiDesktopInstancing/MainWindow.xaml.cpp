#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "ActivationTrackerHelper.h"

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CppWinUiDesktopInstancing::implementation
{
    MainWindow::MainWindow()
    {
        auto numberOfActivations{ GetNumberOfActivations() };
        this->Title(winrt::to_hstring(static_cast<uint32_t>(numberOfActivations)));
        InitializeComponent();
        auto activationTextBox{ activationTrackerTextBox()};
        activationTextBox.Text(winrt::to_hstring(static_cast<uint32_t>(numberOfActivations)));
    }

    void MainWindow::activationTrackerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto activationTextBox{ activationTrackerTextBox() };
        activationTextBox.Text(winrt::to_hstring(static_cast<uint32_t>(GetNumberOfActivations())));
    }
}
