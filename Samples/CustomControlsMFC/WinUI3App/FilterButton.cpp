#include "pch.h"
#include "FilterButton.h"
#if __has_include("FilterButton.g.cpp")
#include "FilterButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    FilterButton::FilterButton()
    {
        InitializeComponent();
    }

    int32_t FilterButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void FilterButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::WinUI3App::implementation::FilterButton::Main_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
