#include "pch.h"
#include "RadioButtonControl.xaml.h"
#if __has_include("RadioButtonControl.g.cpp")
#include "RadioButtonControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    RadioButtonControl::RadioButtonControl()
    {
        InitializeComponent();
    }

    int32_t RadioButtonControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void RadioButtonControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void RadioButtonControl::RadioButtonWidget_Checked(IInspectable const& sender, RoutedEventArgs const& e)
    {
      m_checked(*this, false);
      m_changed(*this, false);
    }


    void RadioButtonControl::RadioButtonWidget_Unchecked(IInspectable const& sender, RoutedEventArgs const& e)
    {
      m_checked(*this, false);
      m_changed(*this, false);
    }


    void RadioButtonControl::RadioButtonWidget_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
      m_clicked(*this, false);
    }

    winrt::hstring RadioButtonControl::text()
    {
      return RadioButtonWidget().Content().as<winrt::hstring>();
    }

    void RadioButtonControl::isChecked(bool check)
    {
      RadioButtonWidget().IsChecked(check);
    }

    bool RadioButtonControl::isChecked()
    {
      return RadioButtonWidget().IsChecked().GetBoolean();
    }

    winrt::event_token RadioButtonControl::clicked(Windows::Foundation::EventHandler<bool> const& handler)
    {
      return m_clicked.add(handler);
    }

    void RadioButtonControl::clicked(winrt::event_token const& handler)
    {
      m_clicked.remove(handler);
    }

    winrt::event_token RadioButtonControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
    {
      return m_changed.add(handler);
    }

    void RadioButtonControl::changed(winrt::event_token const& handler)
    {
      m_changed.remove(handler);
    }

    winrt::event_token RadioButtonControl::checked(Windows::Foundation::EventHandler<bool> const& handler)
    {
      return m_checked.add(handler);
    }

    void RadioButtonControl::checked(winrt::event_token const& handler)
    {
      m_checked.remove(handler);
    }

}
