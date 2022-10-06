#include "pch.h"
#include "CheckBoxControl.xaml.h"
#if __has_include("CheckBoxControl.g.cpp")
#include "CheckBoxControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  CheckBoxControl::CheckBoxControl()
  {
    InitializeComponent();
  }

  int32_t CheckBoxControl::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void CheckBoxControl::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void CheckBoxControl::CheckBoxWidget_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    m_changed(*this, true);
  }


  void CheckBoxControl::CheckBoxWidget_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    m_changed(*this, false);
  }

  void CheckBoxControl::checked(bool checked)
  {
    CheckWidget().IsChecked(checked);
  }

  bool CheckBoxControl::checked()
  {
    return CheckWidget().IsChecked().GetBoolean();
  }

  winrt::event_token CheckBoxControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
  {
    return m_changed.add(handler);
  }

  void CheckBoxControl::changed(winrt::event_token const& handler)
  {
    m_changed.remove(handler);
  }
}
