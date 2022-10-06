#include "pch.h"
#include "ComboBoxControl.xaml.h"
#if __has_include("ComboBoxControl.g.cpp")
#include "ComboBoxControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  ComboBoxControl::ComboBoxControl()
  {
    InitializeComponent();
  }

  int32_t ComboBoxControl::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void ComboBoxControl::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void ComboBoxControl::ComboBoxWidget_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
  {
    m_changed(*this, true);
  }

  void ComboBoxControl::text(const winrt::hstring& str)
  {
    ComboBoxWidget().Text(str);
  }
  winrt::hstring ComboBoxControl::text()
  {
    return ComboBoxWidget().Text();
  }

  int ComboBoxControl::index()
  {
    return ComboBoxWidget().SelectedIndex();
  }
  void ComboBoxControl::index(int i)
  {
    ComboBoxWidget().SelectedIndex(i);
  }

  void ComboBoxControl::addItem(const winrt::hstring& item)
  {
    ComboBoxWidget().Items().Append(box_value(item));
  }

  void ComboBoxControl::clearItems()
  {
    ComboBoxWidget().Items().Clear();
  }

  winrt::event_token ComboBoxControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
  {
    return m_changed.add(handler);
  }
  void ComboBoxControl::changed(winrt::event_token const& handler)
  {
    m_changed.remove(handler);
  }
}
