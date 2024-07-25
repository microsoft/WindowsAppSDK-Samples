#pragma once

#include "MainWindow.g.h"

namespace winrt::WinMainCOMBGTaskSample::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void MainWindow::UnregisterDuplicateTask(_In_ winrt::hstring taskName);
        winrt::hresult MainWindow::RegisterTask(_In_ winrt::Windows::ApplicationModel::Background::IBackgroundTrigger trigger, _In_ hstring taskName, _In_ guid classId, _In_ bool UnregisterOldTask = false);
        void myButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::WinMainCOMBGTaskSample::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
