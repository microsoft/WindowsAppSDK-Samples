#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "TextEditControl.g.h"

namespace winrt::WinUI3App::implementation
{
  struct TextEditControl : TextEditControlT<TextEditControl>
  {
    TextEditControl();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void TextBoxWidget_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
    
    void text(const winrt::hstring& str);
    winrt::hstring text();

    void setPlaceholderText(const winrt::hstring& str);

    winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler);
    void changed(winrt::event_token const& handler);
  private:
    winrt::event<Windows::Foundation::EventHandler<bool>> m_changed;
  };
}

namespace winrt::WinUI3App::factory_implementation
{
  struct TextEditControl : TextEditControlT<TextEditControl, implementation::TextEditControl>
  {
  };
}
