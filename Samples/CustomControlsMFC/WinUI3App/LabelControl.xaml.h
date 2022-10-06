#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "LabelControl.g.h"

namespace winrt::WinUI3App::implementation
{
  struct LabelControl : LabelControlT<LabelControl>
  {
    LabelControl();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void text(const winrt::hstring& str);
    winrt::hstring text();

    void setWordWrap(bool val);

  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct LabelControl : LabelControlT<LabelControl, implementation::LabelControl>
  {
  };
}
