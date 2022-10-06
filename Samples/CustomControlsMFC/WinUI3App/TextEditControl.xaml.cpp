#include "pch.h"
#include "TextEditControl.xaml.h"
#if __has_include("TextEditControl.g.cpp")
#include "TextEditControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  TextEditControl::TextEditControl()
  {
    InitializeComponent();
  }

  int32_t TextEditControl::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void TextEditControl::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void TextEditControl::TextBoxWidget_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e)
  {
  }

  void TextEditControl::text(const winrt::hstring& str)
  {
    TextBoxWidget().Text(str);
  }

  winrt::hstring TextEditControl::text()
  {
    return TextBoxWidget().Text();
  }

  void TextEditControl::setPlaceholderText(const winrt::hstring& str)
  {
    TextBoxWidget().PlaceholderText(str);
  }

  winrt::event_token TextEditControl::changed(Windows::Foundation::EventHandler<bool> const& handler)
  {
    return m_changed.add(handler);
  }

  void TextEditControl::changed(winrt::event_token const& handler)
  {
    m_changed.remove(handler);
  }
}
