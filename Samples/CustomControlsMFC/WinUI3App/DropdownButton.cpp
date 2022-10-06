#include "pch.h"
#include "DropdownButton.h"
#if __has_include("DropdownButton.g.cpp")
#include "DropdownButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    DropdownButton::DropdownButton()
    {
        InitializeComponent();
    }

    int32_t DropdownButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void DropdownButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::WinUI3App::implementation::DropdownButton::Main_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{

}
