#include "pch.h"
#include "SliderControl.xaml.h"
#if __has_include("SliderControl.g.cpp")
#include "SliderControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    SliderControl::SliderControl()
    {
        InitializeComponent();
    }

    int32_t SliderControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SliderControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void SliderControl::SliderWidget_ValueChanged(IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
    {
      m_changed(*this, true);
    }

    void SliderControl::value(int value)
    {
      SliderWidget().Value(value);
    }

    int SliderControl::value()
    {
      return SliderWidget().Value();
    }

    void SliderControl::setMinValue(int value)
    {
      SliderWidget().Minimum(value);
    }

    void SliderControl::setMaxValue(int value)
    {
      SliderWidget().Maximum(value);
    }

    winrt::event_token SliderControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
    {
      return m_changed.add(handler);
    }

    void SliderControl::changed(winrt::event_token const& handler)
    {
      m_changed.remove(handler);
    }

}


