#include "pch.h"
#include "IconButton.h"
#if __has_include("IconButton.g.cpp")
#include "IconButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    IconButton::IconButton()
    {
        InitializeComponent();
    }

    int32_t IconButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void IconButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::WinUI3App::implementation::IconButton::ButtonIcon_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
