#include "pch.h"
#include "NumericControl.xaml.h"
#if __has_include("NumericControl.g.cpp")
#include "NumericControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    NumericControl::NumericControl()
    {
        InitializeComponent();
    }

    int32_t NumericControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void NumericControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void NumericControl::value(double val)
    {
      NumberBoxWidget().Value(val);
    }

    double NumericControl::value()
    {
      return NumberBoxWidget().Value();
    }

    void NumericControl::setMinValue(double val)
    {
      NumberBoxWidget().Minimum(val);
    }

    void NumericControl::setMaxValue(double val)
    {
      NumberBoxWidget().Maximum(val);
    }
}
