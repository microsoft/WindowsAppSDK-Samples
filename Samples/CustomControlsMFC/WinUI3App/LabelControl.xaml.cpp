#include "pch.h"
#include "LabelControl.xaml.h"
#if __has_include("LabelControl.g.cpp")
#include "LabelControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  LabelControl::LabelControl()
  {
    InitializeComponent();
  }

  int32_t LabelControl::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void LabelControl::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  void LabelControl::text(const winrt::hstring& str)
  {
    TextBlockWidget().Text(str);
  }

  winrt::hstring LabelControl::text()
  {
    return TextBlockWidget().Text();
  }

  void LabelControl::setWordWrap(bool val)
  {
    if(val)
      TextBlockWidget().TextWrapping(TextWrapping::Wrap);
    else
      TextBlockWidget().TextWrapping(TextWrapping::Wrap);

  }

}
