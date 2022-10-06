#include "pch.h"
#include "InputButton.h"
#if __has_include("InputButton.g.cpp")
#include "InputButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    InputButton::InputButton()
    {
        InitializeComponent();
    }

    int32_t InputButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void InputButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void winrt::WinUI3App::implementation::InputButton::ButtonInput_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e)
    {

    }
}