#include "pch.h"
#include "StatusButton.h"
#if __has_include("StatusButton.g.cpp")
#include "StatusButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    StatusButton::StatusButton()
    {
        InitializeComponent();
    }

    int32_t StatusButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void StatusButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::WinUI3App::implementation::StatusButton::Main_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
