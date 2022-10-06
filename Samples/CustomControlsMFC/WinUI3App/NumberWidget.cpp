#include "pch.h"
#include "NumberWidget.h"
#if __has_include("NumberWidget.g.cpp")
#include "NumberWidget.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::WinUI3App::implementation
{
    NumberWidget::NumberWidget()
    {
        InitializeComponent();
        /*NumberBoxWidget().IsEnabled(false);
        Down().IsEnabled(false);
        Up().IsEnabled(false);
        UpImage().Opacity(0.50);*/
        Down().Translation(winrt::Windows::Foundation::Numerics::float3(0, 0, 32));
    }

    int32_t NumberWidget::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void NumberWidget::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}


void winrt::WinUI3App::implementation::NumberWidget::Down_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
  NumberBoxWidget().Value(NumberBoxWidget().Value() - 1);
}


void winrt::WinUI3App::implementation::NumberWidget::Up_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
  NumberBoxWidget().Value(NumberBoxWidget().Value() + 1);
}


void winrt::WinUI3App::implementation::NumberWidget::NumberBoxWidget_ValueChanged(winrt::Microsoft::UI::Xaml::Controls::NumberBox const& sender, winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& args)
{

}
