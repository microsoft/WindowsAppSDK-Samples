#include "pch.h"
#include "ContentButton.h"
#if __has_include("ContentButton.g.cpp")
#include "ContentButton.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    ContentButton::ContentButton()
    {
        InitializeComponent();
        ButtonContent().Translation(winrt::Windows::Foundation::Numerics::float3(0, 0, 32));
        //ButtonContent().IsEnabled(true);
    }

    int32_t ContentButton::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ContentButton::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}


void winrt::WinUI3App::implementation::ContentButton::ButtonContent_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
