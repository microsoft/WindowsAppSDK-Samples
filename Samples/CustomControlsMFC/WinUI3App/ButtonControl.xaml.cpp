#include "pch.h"
#include "ButtonControl.xaml.h"
#if __has_include("ButtonControl.g.cpp")
#include "ButtonControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Interop;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  ButtonControl::ButtonControl()
  {
    InitializeComponent();
  }

  int32_t ButtonControl::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void ButtonControl::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void ButtonControl::ButtonWidget_Click(IInspectable const&, RoutedEventArgs const&)
  {
    m_clicked(*this, true);
  }

  void ButtonControl::text(const winrt::hstring& str)
  {
    ButtonWidget().Content(box_value(str));
  }

  void ButtonControl::icon(const winrt::hstring& str)
  {
    //TODO
    //Windows::Foundation::Uri uri{ str };
    //System::Windows::Media::Imaging::BitmapImage bitmapImage;
    //bitmapImage.SetSource(uri);
    //ImageWidget().Source(bitmapImage);
  }
  
  winrt::event_token ButtonControl::clicked(Windows::Foundation::EventHandler<bool> const& handler)
  {
    return m_clicked.add(handler);
  }

  void ButtonControl::clicked(winrt::event_token const& handler)
  {
    m_clicked.remove(handler);
  }


}
