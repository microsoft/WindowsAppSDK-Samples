// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ReunionCppDesktopSampleApp::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::CallAPIButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        winrt::hstring resultMessage = L"API call succeeded.";
        try
        {
            // The purpose of this simple sample is to show the setup for ingesting the Project Reunion
            // Nuget. At the moment there are not very many APIs exposed through Project Reunion.
            // The follow line is just calling an arbitrary method on an activatable class that is
            // included in Project Reunion as validation/demonstration of things working end to end.
            auto activationArgs = winrt::Microsoft::ProjectReunion::AppLifecycle::GetActivatedEventArgs();
        }
        catch (winrt::hresult_error const& ex)
        {
            resultMessage = L"API call failed with error: " + ex.message();
        }

        resultText().Text(resultMessage);
    }
}
